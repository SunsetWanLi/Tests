LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/GUI
LOCAL_MODULE:= GUI

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
		$(Z11_PATH)


LOCAL_SRC_FILES:= \
	Animations/MIAnimationsManager.cpp \
	Animations/MIAnimationAlgorithm.cpp \
	Animations/MIAnimationLayer.cpp \
	Base/MIMultiReusableQueue.cpp \
	Base/MIReusableQueue.cpp \
	Base/MIObject.cpp \
	Base/MIResponder.cpp \
	Base/MITouchTracer.cpp \
	Base/MIView.cpp \
	Controllers/MIViewController.cpp \
	Controllers/MINavigationController.cpp \
    Drama/MGActor.cpp \
	Events/MIEvent.cpp \
	Events/MIEventQueue.cpp \
	Events/MITouch.cpp \
    GestureRecognizer/MIGestureRecognizer.cpp \
	GestureRecognizer/MILongPressGestureRecognizer.cpp \
	GestureRecognizer/MIPanGestureRecognizer.cpp \
	GestureRecognizer/MISwipeGestureRecognizer.cpp \
	GestureRecognizer/MIPinchGestureRecognizer.cpp \
	GestureRecognizer/MITapGestureRecognizer.cpp \
	GestureRecognizer/MIRotationGestureRecognizer.cpp \
	GuiContainer/GuiContainer.cpp \
	LightGraphic/LightGraphic.cpp \
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
    MIControls/MIScaleView.cpp \
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
	MIControls/MITextButton.cpp \
	MIControls/MITextEditMenu.cpp \
	MIControls/MITextField.cpp \
	MIControls/MITextView.cpp \
    MICustomControls/ImageResizableButton.cpp \
	MICustomControls/MIAlertDialog.cpp \
    MICustomControls/MIRainMonitor.cpp \
    MICustomControls/ToastView.cpp \
	MICustomEngine/CustomizeDocument.cpp \
	MICustomEngine/CustomizeNode.cpp \
	MILayoutEngine/MILayoutDocument.cpp \
	MILayoutEngine/MILayoutEngine.cpp \
	MILayoutEngine/MILayoutNode.cpp

#	MIControls/MIRasterImage.cpp \  
LOCAL_STATIC_LIBRARIES := CommonBase System wchar_static

include $(BUILD_STATIC_LIBRARY)
