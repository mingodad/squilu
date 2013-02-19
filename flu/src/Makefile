include ../makeinclude

%.o : %.cpp
	@ echo Compiling $@...; $(CXX) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	@ echo Compiling $@...; $(CC) $(CXXFLAGS) -c $< -o $@

CPP_SOURCE = \
Flu_Button.cpp \
Flu_Chat_Buffer.cpp \
Flu_Choice_Group.cpp \
Flu_Collapsable_Group.cpp \
Flu_Combo_Box.cpp \
Flu_Combo_List.cpp \
Flu_Combo_Tree.cpp \
Flu_DND.cpp \
Flu_Dual_Progress_Meter.cpp \
Flu_Dual_Slider.cpp \
Flu_File_Chooser.cpp \
Flu_Float_Input.cpp \
Flu_Helpers.cpp \
Flu_Int_Input.cpp \
Flu_Label.cpp \
Flu_Link_Button.cpp \
Flu_Output.cpp \
flu_file_chooser_pixmaps.cpp \
flu_pixmaps.cpp \
Flu_Progress.cpp \
Flu_Progress_Meter.cpp \
Flu_Return_Button.cpp \
Flu_Separator.cpp \
Flu_Simple_Group.cpp \
FluSimpleString.cpp \
Flu_Spinner.cpp \
Flu_Toggle_Group.cpp \
Flu_Tree_Browser.cpp \
Flu_Wrap_Group.cpp \

GL_CPP_SOURCE = \
Flu_GL_Canvas.cpp \
Flu_GL_Window.cpp

OBJS = $(CPP_SOURCE:.cpp=.o)
GL_OBJS = $(GL_CPP_SOURCE:.cpp=.o)

STATIC_LIB_DIR = ../lib

SHARED_LINK = libflu.$(SHARED_EXT)
SHARED_LIB = $(SHARED_LINK).$(VERSION)
STATIC_LIB = libflu.a
GL_SHARED_LINK = libflu_gl.$(SHARED_EXT)
GL_SHARED_LIB = $(GL_SHARED_LINK).$(VERSION)
GL_STATIC_LIB = libflu_gl.a

LIBS = $(STATIC_LIB)
ifeq ($(ENABLE_SHARED),1)
	LIBS += $(SHARED_LIB)
endif
ifeq ($(ENABLE_GL),1)
	LIBS += $(GL_STATIC_LIB)
	ifeq ($(ENABLE_SHARED),1)
		LIBS += $(GL_SHARED_LIB)
	endif
endif

all:	$(LIBS)

$(SHARED_LIB):	$(OBJS)
	@ echo Creating $@...; $(CXX) $(SHARED_FLAG) -o $(SHARED_LIB) $(OBJS) $(LDFLAGS) $(LDFLAGS_SHARED) $(EXTRAS)
	@ ln -sf $(SHARED_LIB) $(SHARED_LINK)

$(STATIC_LIB):	$(OBJS)
	@ echo Creating $@...; $(ARCHIVE) $(STATIC_LIB) $(OBJS)
	@ cp -f $(STATIC_LIB) $(STATIC_LIB_DIR)

$(GL_SHARED_LIB):	$(GL_OBJS)
	@ echo Creating $@...; $(CXX) $(SHARED_FLAG) -o $(GL_SHARED_LIB) $(GL_OBJS) $(LDFLAGS) $(LDFLAGS_SHARED) $(GL_EXTRAS)
	@ ln -sf $(GL_SHARED_LIB) $(GL_SHARED_LINK)

$(GL_STATIC_LIB):	$(GL_OBJS)
	@ echo Creating $@...; $(ARCHIVE) $(GL_STATIC_LIB) $(GL_OBJS)
	@ cp -f $(GL_STATIC_LIB) $(STATIC_LIB_DIR)

clean:
	-@ rm -rf *.o ii_files so_locations *.so* $(STATIC_LIB_DIR)/$(STATIC_LIB) $(STATIC_LIB) $(SHARED_LIB) $(SHARED_LINK) $(STATIC_LIB_DIR)/$(GL_STATIC_LIB) $(GL_STATIC_LIB) $(GL_SHARED_LIB) $(GL_SHARED_LINK)


install:	$(LIBS)
	@ echo "Installing libraries in $(LPREFIX)..."
	@ mkdir -p $(LPREFIX)
	@ rm -f $(LPREFIX)/$(STATIC_LIB) $(LPREFIX)/$(GL_STATIC_LIB) $(LPREFIX)/$(SHARED_LINK) $(LPREFIX)/$(GL_SHARED_LINK) $(LPREFIX)/$(SHARED_LIB) $(LPREFIX)/$(GL_SHARED_LIB)
	@ for file in $(LIBS); do \
		cp $$file $(LPREFIX)/; \
		chmod 755 $(LPREFIX)/$$file; \
		if test $$file = $(SHARED_LIB); then \
			ln -sf $(SHARED_LIB) $(LPREFIX)/$(SHARED_LINK); \
		fi ; \
		if test $$file = $(GL_SHARED_LIB); then \
			ln -sf $(GL_SHARED_LIB) $(LPREFIX)/$(GL_SHARED_LINK); \
		fi; \
	  done

uninstall:
	@ echo "Uninstalling libraries from $(LPREFIX)..."
	@ rm -rf $(LPREFIX)/libflu*

depend:	$(CPP_SOURCE) $(GL_CPP_SOURCE) 
	makedepend -Y -I../ -f makedepend $(CPP_SOURCE) $(GL_CPP_SOURCE) 

include makedepend
