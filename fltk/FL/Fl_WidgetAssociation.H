#ifndef fltk_WidgetAssociation_h
#define fltk_WidgetAssociation_h

#include <FL/Fl_Export.H>

class Fl_Widget;

/** Base class for the association type.
 *
 * FLTK allows you to attach any kind of user data to a widget. This data is automatically freed when the
 * widget to which it is attached is destroyed. To identify tha attached data an instance of a subclass
 * of this class is used. Each instance of attached data needs its own instance of a subclass.
 * It is not possibel to attach multiple instances of the same type.
 *
 * The subclass is also used to free the data, once the widget that the data is attached to is deleted.
 * When that happens all attached data is destroyed using the destroy function within this class. As
 * each attached data package has its own pointer to an association type it can also have its own
 * function to free the data.
 *
 * possible uses:
 * - assign key shortcuts to certain widgets
 * - assign a tooltip to some widgets
 * - assign a help-index to widgets
 * - assign a unique identifier to widgets to remote controlling
 * - assign additional layouting data for new container widgets
 * - assign data needed by typesafe callback mechanisms
 * - assign all kind of data not always required within a widget / each widget
 */
class Fl_Association_Type {

  public:
    /** This function is called when associated data is freed.
     *
     * This function must be proveded when creating a data specific subclass. The function
     * must do whatever is necessary to free associated data. Most of the time it will be a cast
     * to the right datatype and a delete
     */
    virtual void destroy(void* data) const = 0;
};

/** Fl_AssociationFunctor Class used by the foreach() functions.
 *
 * This is the abstract base class for the association functor that is used in foreach().
 * If you want to supply your specific actions to do with the associated data found by the 
 * foreach() functions you need to derive from this class and provide a new handle function.
 */
class Fl_Association_Functor {
  public:
    /** For each found association this function is called.
     * If the function returns true the
     * loop is aborted and the data pointer for the current association is returned.
     * If you want to iterator over all associations simply don't return true, but
     * only do whatever you need within this function.
     */
    virtual bool handle(const Fl_Association_Type&, const Fl_Widget*, void* data) = 0;
};

/** \relates Fl_AssociationType
 * This function allows traversing all associations of a certain association type, a certain widget,
 * both, or none of the constraints.
 *
 * For each found widget the handle function in the associaionFunctor class is called. If that
 * function returns true the scan is aborted and the data for the current widget is returned
 * A NULL pointer for the Fl_AssociationType or the Fl_Widget pointer means to call the functor for all
 * AssociationTypes and/or all Widgets.
 * Important: don't modify any associations while this function is running!
 *
 * The function either returns the first associated data for which the functor returns true, or NULL.
 * \param[in] at the type of associations you want to iterate over, if NULL all associations will be taken
 * \param[in] w the widget you want to iterate over, if NULL all widgets will be taken
 * \param[in] ftk the functor object that contains the handle function to be called for all associations found
 */
void* AssocForeach(const Fl_Association_Type* at, const Fl_Widget* w, Fl_Association_Functor& fkt);

#endif
