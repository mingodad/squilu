#include <FL/Fl_WidgetAssociation.H>
#include <FL/Fl_Widget.H>

#include <string.h>

/* implementation:
 *   hashtable over the primary key (the widget)
 *   each table entry is for exactly one widget and contains a list
 *     of all associations for that widget
 *   this allows for fast lookup of many widgets, but assumes that not all
 *   that much data is associated with one widget, in case applications come up
 *   with too much data for one widget we need to rectify this
 */

/* instead of a linked list we could use an array here to save some memory
 */
struct Association {
  const Fl_Association_Type* at;
  void* data;
  Association* next;
};

struct PrimaryAssociation {
  const Fl_Widget* wg;
  Association* associations;
  PrimaryAssociation* next;
};

static PrimaryAssociation** associationTable = 0;
static size_t associationTableSize = 0;
static size_t associationTableEntries = 0;

static void growTable(void) {

  if (associationTableEntries >= associationTableSize) {

    size_t newassociationTableSize;

    // increase table size and rehash
    if (associationTableSize == 0) {
      newassociationTableSize = 23;
    } else {
      newassociationTableSize = 2 * associationTableSize + 1;
    }

    PrimaryAssociation** newTab = new PrimaryAssociation*[newassociationTableSize];
    memset(newTab, 0, newassociationTableSize*sizeof(PrimaryAssociation*));

    for (size_t i = 0; i < associationTableSize; i++)
      while (associationTable[i]) {
        size_t const hash = (size_t)associationTable[i]->wg % newassociationTableSize;
        PrimaryAssociation* node = associationTable[i];
        associationTable[i] = node->next;

        node->next = newTab[hash];
        newTab[hash] = node;
      }

    if (associationTableSize)
      delete [] associationTable;

    associationTable = newTab;
    associationTableSize = newassociationTableSize;
  }
}

void Fl_Widget::set_assoc(const Fl_Association_Type& at, void* data) const {

  remove_assoc(at);

  if (!data) return;

  growTable();

  size_t hash = (size_t)this % associationTableSize;

  PrimaryAssociation* node = associationTable[hash];

  while (node && node->wg != this) node = node->next;

  if (!node) {

    node = new PrimaryAssociation;
    node->next = associationTable[hash];
    node->wg = this;
    node->associations = 0;
    associationTable[hash] = node;

    associationTableEntries++;
  }

  /* add new association for this type */
  Association* ass = new Association;
  ass->next = node->associations;
  ass->at = &at;
  ass->data = data;
  node->associations = ass;
}

void* Fl_Widget::get_assoc(const Fl_Association_Type& at) const {

  if (associationTableSize == 0) return 0;

  size_t hash = (size_t)this % associationTableSize;

  PrimaryAssociation* node = associationTable[hash];

  while (node && node->wg != this) node = node->next;

  if (!node) return 0;

  Association* ass = node->associations;

  while (ass && ass->at != &at) ass = ass->next;

  if (!ass) return 0;

  return ass->data;
}

void Fl_Widget::remove_assoc(const Fl_Association_Type& at) const {

  if (associationTableEntries > 0) {

    size_t hash = (size_t)this % associationTableSize;

    PrimaryAssociation* node = associationTable[hash];

    while (node && node->wg != this) node = node->next;

    if (node) {

      /* remove all associations for this associationtype */
      Association* ass = node->associations;
      Association* prevass = 0;

      while (ass) {

        if (ass->at == &at) {

          if (prevass) {
            prevass->next = ass->next;
          } else {
            node->associations = ass->next;
          }

          ass->at->destroy(ass->data);
          Association* an = ass->next;
          delete ass;
          ass = an;

        } else {

          prevass = ass;
          ass = ass->next;
        }
      }
    }
  }
}

// This is an internal function that is called by ~Widget().
// It destroys all the associations with the widget.
void delete_associations_for(Fl_Widget* widget) {

  if (associationTableSize == 0) return;

  const size_t hash = (size_t)widget % associationTableSize;

  PrimaryAssociation* node = associationTable[hash];
  PrimaryAssociation* prev = 0;

  while (node && node->wg != widget) {
    prev = node;
    node = node->next;
  }

  if (!node) return;

  while (node->associations) {

    Association* an = node->associations->next;
    node->associations->at->destroy(node->associations->data);
    delete node->associations;
    node->associations = an;
  }

  if (prev) {
    prev->next = node->next;
  } else {
    associationTable[hash] = node->next;
  }

  associationTableEntries--;

  delete node;
}

/* iterate over all associations of one widget */
static void* AssocForeach(const PrimaryAssociation* node, const Fl_Association_Type* at, Fl_Association_Functor& fkt) {

  Association* ass = node->associations;

  while (ass) {

    if (!at || ass->at == at)
    {
      bool stop = fkt.handle(*(ass->at), node->wg, ass->data);

      if (stop)
        return ass->data;
    }

    ass = ass->next;
  }

  return 0;
}

void* AssocForeach(const Fl_Association_Type* at, const Fl_Widget* wg, Fl_Association_Functor& fkt) {

  if (associationTableSize == 0) return 0;

  if (wg) {

    const size_t hash = (size_t)wg % associationTableSize;
    PrimaryAssociation* node = associationTable[hash];

    while (node && node->wg != wg) node = node->next;

    if (!node) return 0;

    return AssocForeach(node, at, fkt);

  } else {

    for (unsigned int i = 0; i < associationTableSize; i++) {

      PrimaryAssociation* node = associationTable[i];

      while (node) {

        void * res = AssocForeach(node, at, fkt);

        if (res) return res;

        node = node->next;
      }
    }
  }

  return 0;
}
