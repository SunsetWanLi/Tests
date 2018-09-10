MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "This is ROOT_DIR dir " ${ROOT_DIR})
SET(PROJECT_SOURCE_DIR ${ROOT_DIR}/../../GUI )
SET(SRC_LIST, \ 
	Animations/MIAnimationsManager.cpp \
	Animations/MIAnimationAlgorithm.cpp \
	Animations/MIAnimationLayer.cpp \
	Base/MIObject.cpp \
	Base/MIResponder.cpp \
	Base/MITouchTracer.cpp \
	Base/MIView.cpp \
	Controllers/MIViewController.cpp \
	Controllers/MINavigationController.cpp \
	Events/MIEvent.cpp \
	Events/MIEventQueue.cpp \
	Events/MITouch.cpp \
	GuiContainer/GuiContainer.cpp \
	LightGraphic/LightGraphic.cpp \
	MIControls/MIActionSheet.cpp \
	MIControls/MIActivityIndicatorView.cpp \
	MIControls/MIAlertView.cpp \
	MIControls/MIBarButtonItem.cpp \
	MIControls/MIBarItem.cpp \
	MIControls/MIBaseTextView.cpp \
	MIControls/MIButton.cpp \
	MIControls/MIControl.cpp \
	MIControls/MICursor.cpp \
	MIControls/MIDatePicker.cpp \
	MIControls/MIEditableTextView.cpp \
	MIControls/MIImageView.cpp \
	MIControls/MILabel.cpp \
	MIControls/MINavigationBar.cpp \
	MIControls/MIPickerView.cpp \
	MIControls/MIPictureViewCell.cpp \
	MIControls/MIPictureView.cpp \
	MIControls/MIProgressView.cpp \
	MIControls/MIRoundedRectButton.cpp \
	MIControls/MIScrollView.cpp \
	MIControls/MISearchBar.cpp \
	MIControls/MISegmentedControl.cpp \
	MIControls/MISlide.cpp \
	MIControls/MIScreenCaptureView.cpp \
	MIControls/MISwitch.cpp \
	MIControls/MITableViewCellAccessoryButton.cpp \
	MIControls/MITableViewCell.cpp \
	MIControls/MITableView.cpp \
	MIControls/MITemplatePickerDelegate.cpp \
	MIControls/MITemplateTableCell.cpp \
	MIControls/MITextArea.cpp \
	MIControls/MITextAreaCell.cpp \
	MIControls/MITextButton.cpp \
	MIControls/MITextField.cpp \
	MIControls/MITextView.cpp \
	MILayoutEngine/MILayoutDocument.cpp \
	MILayoutEngine/MILayoutEngine.cpp \
	MILayoutEngine/MILayoutNode.cpp \
	GestureRecognizer/MIGestureRecognizer.cpp \
	GestureRecognizer/MIPanGestureRecognizer.cpp \
	GestureRecognizer/MIPinchGestureRecognizer.cpp \
	Drama/MGActor.cpp)
MESSAGE(STATUS "This is SOURCE_LIST dir " ${SRC_LIST})
ADD_LIBRARY(gui STATIC ${SRC_LIST})
SET(LIBRARY_OUTPUT_PATH ${ROOT_DIR}/builds/libs)
