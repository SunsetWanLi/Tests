//
//  EAGLView.h
//  OpenGLES_iPhone
//
//  Created by mmalc Crawford on 11/18/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface APLIndexedPosition : UITextPosition

@property (nonatomic) NSUInteger index;
//@property (nonatomic, assign) id <UITextInputDelegate> inputDelegate;

+ (instancetype)positionWithIndex:(NSUInteger)index;

@end

@interface APLIndexedRange : UITextRange

@property (nonatomic) NSRange range;
+ (instancetype)indexedRangeWithRange:(NSRange)range;

@end

@class EAGLContext;

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView <UITextInput> {
    id <UITextInputDelegate>        _inputDelegate;
    UIKeyboardAppearance            _keyboardAppearance;
    UITextAutocapitalizationType    _autocapitalizationType;
    UIKeyboardType                  _keyboardType;
    UIReturnKeyType                 _returnKeyType;
    UITextInputStringTokenizer      *_tokenizer;
    UIViewController                *_controller;
    UIImageView                     *loader_view;
    BOOL                            accept_keyboard_input;
}

@property(nonatomic, retain) UIViewController *_controller;
@property(nonatomic, assign) UIImageView *loader_view;
@property (nonatomic) BOOL accept_keyboard_input;
@property (nonatomic) NSRange marked_text_range;
@property (nonatomic, assign) NSMutableString *text;

@end