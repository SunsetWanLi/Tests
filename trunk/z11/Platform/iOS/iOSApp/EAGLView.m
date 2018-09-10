//
//  EAGLView.m
//  OpenGLES_iPhone
//
//  Created by mmalc Crawford on 11/18/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "EAGLView.h"

@implementation EAGLView

@synthesize markedTextStyle = _markedTextStyle;
@synthesize inputDelegate = _inputDelegate;
@synthesize keyboardAppearance = _keyboardAppearance;
@synthesize autocapitalizationType = _autocapitalizationType;
@synthesize keyboardType = _keyboardType;
@synthesize returnKeyType = _returnKeyType;

@synthesize _controller;
@synthesize loader_view;
@synthesize accept_keyboard_input;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithCoder:(NSCoder*)coder
{
    self.loader_view = nil;
    self = [super initWithCoder:coder];
    self.text = [[NSMutableString alloc] init];
	if (self) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = YES;
        
        CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
        if(CGSizeEqualToSize(CGSizeMake(640, 960), screen_size) || 
           CGSizeEqualToSize(CGSizeMake(1536, 2048), screen_size) || CGSizeEqualToSize(CGSizeMake(640, 1136), screen_size) || CGSizeEqualToSize(CGSizeMake(2048, 1536), screen_size) || CGSizeEqualToSize(CGSizeMake(750, 1334), screen_size) ||
           CGSizeEqualToSize(CGSizeMake(1334, 750), screen_size))
        {
            eaglLayer.contentsScale = 2;
            //self.contentScaleFactor = MAX(1.0f, [[UIScreen mainScreen] scale]);
        } else if (CGSizeEqualToSize(CGSizeMake(1242, 2208), screen_size) || CGSizeEqualToSize(CGSizeMake(2208, 1242), screen_size) || CGSizeEqualToSize(CGSizeMake(1125, 2001), screen_size) || CGSizeEqualToSize(CGSizeMake(2001, 1125), screen_size))
        {
            eaglLayer.contentsScale = 3;
        }
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
        _tokenizer = [[UITextInputStringTokenizer alloc] initWithTextInput:self];
        _keyboardAppearance = UIKeyboardAppearanceDefault;
        _autocapitalizationType = UITextAutocapitalizationTypeNone;
        _keyboardType = UIKeyboardTypeDefault;
        _returnKeyType = UIReturnKeyDefault;
        accept_keyboard_input = NO;
    }
    
    return self;
}

- (void)dealloc
{
    self.markedTextStyle = nil;
    [_tokenizer release];
    _tokenizer = nil;
    [self.text release];
    self.text = nil;
    [super dealloc];
}

#pragma mark - Responder

- (BOOL)becomeFirstResponder
{
    [super becomeFirstResponder];
    self.accept_keyboard_input = YES;
    self.marked_text_range = NSMakeRange(NSNotFound, 0);
    return YES;
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (BOOL)canResignFirstResponder
{
    return YES;
}// default is YES
- (BOOL)resignFirstResponder
{
    [super resignFirstResponder];
    self.accept_keyboard_input = NO;
    [self.text deleteCharactersInRange:NSMakeRange(0, self.text.length)];
    self.marked_text_range = NSMakeRange(NSNotFound, 0);
    return YES;
}

#pragma mark UITextInput methods

#pragma mark UITextInput - Replacing and Returning Text

// UITextInput required method - called by text system to get the string for
// a given range in the text storage
- (NSString *)textInRange:(UITextRange *)range
{
//    NSLog(@"=================================");
//    NSLog(@"textInRange");
//    NSLog(@"textInRange range:%@", range);
    APLIndexedRange *r = (APLIndexedRange *)range;
    return ([self.text substringWithRange:r.range]);
}

// UITextInput required method - called by text system to replace the given
// text storage range with new text
- (void)replaceRange:(UITextRange *)range withText:(NSString *)text
{
//    NSLog(@"=================================");
//    NSLog(@"replaceRange");
//    NSLog(@"replaceRange range:%@, text:%@", range, text);
    APLIndexedRange *indexedRange = (APLIndexedRange *)range;
    
    // Now replace characters in text storage
    [self.text replaceCharactersInRange:indexedRange.range withString:text];
}

#pragma mark UITextInput - Working with Marked and Selected Text

// UITextInput selectedTextRange property accessor overrides
// (access/update underlaying SimpleCoreTextView)

- (UITextRange *)selectedTextRange
{
//    NSLog(@"=================================");
//    NSLog(@"selectedTextRange");
    return [APLIndexedRange indexedRangeWithRange:NSMakeRange(NSNotFound, 0)];
}

- (void)setSelectedTextRange:(UITextRange *)range
{
//    NSLog(@"=================================");
//    NSLog(@"setSelectedTextRange");
//    NSLog(@"setSelectedTextRange range:%@", range);
    return;
}

// UITextInput markedTextRange property accessor overrides
// (access/update underlaying SimpleCoreTextView)

- (UITextRange *)markedTextRange
{
//    NSLog(@"=================================");
//    NSLog(@"markedTextRange");
    if (self.marked_text_range.length == 0) {
        return nil;
    }
    
    return [APLIndexedRange indexedRangeWithRange:self.marked_text_range];
}

// UITextInput required method - Insert the provided text and marks it to indicate
// that it is part of an active input session.
- (void)setMarkedText:(NSString *)markedText selectedRange:(NSRange)selectedRange
{
    if (markedText == nil)
        markedText = @"";
    
    NSRange markedTextRange = self.marked_text_range;
    if (self.marked_text_range.location != NSNotFound) {
        // Replace characters in text storage and update markedText range length.
        [self.text replaceCharactersInRange:markedTextRange withString:markedText];
        markedTextRange.length = markedText.length;
    } else if (markedText.length > 0) {
        [self.text appendString:markedText];
        markedTextRange.location = 0;
        markedTextRange.length = markedText.length;
    }
    
    self.marked_text_range = markedTextRange;
    
    [_controller setMarkedText:markedText selectedRange:selectedRange];
}

// UITextInput required method - Unmark the currently marked text.
- (void)unmarkText
{
    NSRange markedTextRange = self.marked_text_range;
    
    if (markedTextRange.location == NSNotFound) {
        return;
    }
    // Unmark the underlying APLSimpleCoreTextView.markedTextRange.
    markedTextRange.location = NSNotFound;
    self.marked_text_range = markedTextRange;
    [_controller unmarkText];
}

#pragma mark UITextInput - Computing Text Ranges and Text Positions

// UITextInput beginningOfDocument property accessor override
- (UITextPosition *)beginningOfDocument
{
//    NSLog(@"=================================");
//    NSLog(@"beginningOfDocument");
	return [APLIndexedPosition positionWithIndex:0];;
}

// UITextInput endOfDocument property accessor override
- (UITextPosition *)endOfDocument
{
//    NSLog(@"=================================");
//    NSLog(@"endOfDocument");
	return [APLIndexedPosition positionWithIndex:self.text.length];
}

// UITextInput protocol required method - Return the range between two text positions
// using our implementation of UITextRange
- (UITextRange *)textRangeFromPosition:(UITextPosition *)fromPosition toPosition:(UITextPosition *)toPosition
{
//    NSLog(@"=================================");
//    NSLog(@"textRangeFromPosition");
//    NSLog(@"textRangeFromPosition fromPosition:%@ toPosition:%@", fromPosition, toPosition);
	return nil;
    
}

// UITextInput protocol required method - Returns the text position at a given offset
// from another text position using our implementation of UITextPosition
- (UITextPosition *)positionFromPosition:(UITextPosition *)position offset:(NSInteger)offset
{
//    NSLog(@"=================================");
//    NSLog(@"positionFromPosition");
//    NSLog(@"positionFromPosition position%@ offset:%d", position, offset);
	return nil;
}

// UITextInput protocol required method - Returns the text position at a given offset
// in a specified direction from another text position using our implementation of
// UITextPosition.
- (UITextPosition *)positionFromPosition:(UITextPosition *)position inDirection:(UITextLayoutDirection)direction offset:(NSInteger)offset
{
//    NSLog(@"=================================");
//    NSLog(@"positionFromPosition");
//    NSLog(@"positionFromPosition position:%@ direction:%d offset:%d", position, direction, offset);
    return nil;
}

#pragma mark UITextInput - Evaluating Text Positions

// UITextInput protocol required method - Return how one text position compares to another
// text position.
- (NSComparisonResult)comparePosition:(UITextPosition *)position toPosition:(UITextPosition *)other
{
//    NSLog(@"=================================");
//    NSLog(@"comparePosition");
//    NSLog(@"comparePosition position:%@ toPosition:%@", position, other);
    return NSOrderedDescending;
}

// UITextInput protocol required method - Return the number of visible characters
// between one text position and another text position.
- (NSInteger)offsetFromPosition:(UITextPosition *)from toPosition:(UITextPosition *)toPosition
{
//    NSLog(@"=================================");
//    NSLog(@"offsetFromPosition");
//    NSLog(@"offsetFromPosition from:%@ toPosition:%@", from, toPosition);
    return 0;
}

#pragma mark UITextInput - Text Input Delegate and Text Input Tokenizer

// UITextInput tokenizer property accessor override
//
// An input tokenizer is an object that provides information about the granularity
// of text units by implementing the UITextInputTokenizer protocol.  Standard units
// of granularity include characters, words, lines, and paragraphs. In most cases,
// you may lazily create and assign an instance of a subclass of
// UITextInputStringTokenizer for this purpose, as this sample does. If you require
// different behavior than this system-provided tokenizer, you can create a custom
// tokenizer that adopts the UITextInputTokenizer protocol.
- (id <UITextInputTokenizer>)tokenizer
{
//    NSLog(@"=================================");
//    NSLog(@"tokenizer");
    return _tokenizer;
}

#pragma mark UITextInput - Text Layout, writing direction and position related methods

// UITextInput protocol method - Return the text position that is at the farthest
// extent in a given layout direction within a range of text.
- (UITextPosition *)positionWithinRange:(UITextRange *)range farthestInDirection:(UITextLayoutDirection)direction
{
//    NSLog(@"=================================");
//    NSLog(@"positionWithinRange");
//    NSLog(@"positionWithinRange range:%@ farthestInDirection:%d", range, direction);
    return nil;
}

// UITextInput protocol required method - Return a text range from a given text position
// to its farthest extent in a certain direction of layout.
- (UITextRange *)characterRangeByExtendingPosition:(UITextPosition *)position inDirection:(UITextLayoutDirection)direction
{
//    NSLog(@"=================================");
//    NSLog(@"characterRangeByExtendingPosition");
//    NSLog(@"characterRangeByExtendingPosition position:%@ inDirection:%d", position, direction);
    return nil;
}

// UITextInput protocol required method - Return the base writing direction for
// a position in the text going in a specified text direction.
- (UITextWritingDirection)baseWritingDirectionForPosition:(UITextPosition *)position inDirection:(UITextStorageDirection)direction
{
    // This sample assumes LTR text direction and does not currently support BiDi or RTL.
//    NSLog(@"=================================");
//    NSLog(@"baseWritingDirectionForPosition");
//    NSLog(@"baseWritingDirectionForPosition position:%@ inDirection:%d", position, direction);
    return UITextWritingDirectionLeftToRight;
}

// UITextInput protocol required method - Set the base writing direction for a
// given range of text in a document.
- (void)setBaseWritingDirection:(UITextWritingDirection)writingDirection forRange:(UITextRange *)range
{
//    NSLog(@"=================================");
//    NSLog(@"setBaseWritingDirection");
//    NSLog(@"setBaseWritingDirection writingDirection:%d forRange:%@", writingDirection, range);
    // This sample assumes LTR text direction and does not currently support BiDi or RTL.
}

#pragma mark UITextInput - Geometry methods

// UITextInput protocol required method - Return the first rectangle that encloses
// a range of text in a document.
- (CGRect)firstRectForRange:(UITextRange *)range
{
//    NSLog(@"=================================");
//    NSLog(@"firstRectForRange");
//    NSLog(@"firstRectForRange range:%@", range);
    return CGRectMake(0, 0, 320, 320);
}

// UITextInput protocol required method - Return a rectangle used to draw the caret
// at a given insertion point.
- (CGRect)caretRectForPosition:(UITextPosition *)position
{
//    NSLog(@"=================================");
//    NSLog(@"caretRectForPosition");
//    NSLog(@"caretRectForPosition position:%@", position);
    return CGRectMake(0, 0, 20, 320);
}

#pragma mark UITextInput - Hit testing

// Note that for this sample hit testing methods are not implemented, as there
// is no implemented mechanic for letting user select text via touches.  There
// is a wide variety of approaches for this (gestures, drag rects, etc) and
// any approach chosen will depend greatly on the design of the application.

// UITextInput protocol required method - Return the position in a document that
// is closest to a specified point.
- (UITextPosition *)closestPositionToPoint:(CGPoint)point
{
//    NSLog(@"=================================");
//    NSLog(@"closestPositionToPoint");
//    NSLog(@"closestPositionToPoint point.x:%f point.y:%f", point.x, point.y);
	// Not implemented in this sample.  Could utilize underlying
	// SimpleCoreTextView:closestIndexToPoint:point
    return nil;
}

// UITextInput protocol required method - Return the position in a document that
// is closest to a specified point in a given range.
- (UITextPosition *)closestPositionToPoint:(CGPoint)point withinRange:(UITextRange *)range
{
//    NSLog(@"=================================");
//    NSLog(@"closestPositionToPoint");
//    NSLog(@"closestPositionToPoint point.x:%f point.y:%f withinRange:%@", point.x, point.y, range);
	// Not implemented in this sample.  Could utilize underlying
	// SimpleCoreTextView:closestIndexToPoint:point
    return nil;
}

// UITextInput protocol required method - Return the character or range of
// characters that is at a given point in a document.
- (UITextRange *)characterRangeAtPoint:(CGPoint)point
{
//    NSLog(@"=================================");
//    NSLog(@"characterRangeAtPoint");
//    NSLog(@"characterRangeAtPoint point.x:%f point.y:%f", point.x, point.y);
	// Not implemented in this sample.  Could utilize underlying
	// SimpleCoreTextView:closestIndexToPoint:point
    return nil;
}

/*
 UITextInput protocol required method.
 Return an array of UITextSelectionRects.
 */
- (NSArray *)selectionRectsForRange:(UITextRange *)range
{
    // Not implemented in this sample.
    return nil;
}

#pragma mark UITextInput - Returning Text Styling Information

// UITextInput protocol method - Return a dictionary with properties that specify
// how text is to be style at a certain location in a document.
- (NSDictionary *)textStylingAtPosition:(UITextPosition *)position inDirection:(UITextStorageDirection)direction
{
//    NSLog(@"=================================");
//    NSLog(@"textStylingAtPosition");
//    NSLog(@"textStylingAtPosition position:%@ inDirection:%d", position, direction);
    // This sample assumes all text is single-styled, so this is easy.
    return nil;//[NSDictionary dictionaryWithObject:_textView.font forKey:UITextInputTextFontKey];
}

#pragma mark UIKeyInput methods

- (BOOL)hasText
{
    return [_controller hasText];
}
- (void)insertText:(NSString *)text
{
    [_controller insertText:text];
}
- (void)deleteBackward
{
    [_controller deleteBackward];
}

- (void)setKeyboardType:(UIKeyboardType)keyboardType
{
    _keyboardType = keyboardType;
}

- (void)setReturnKeyType:(UIReturnKeyType)returnKeyType
{
    _returnKeyType = returnKeyType;
}

@end

@implementation APLIndexedPosition

#pragma mark IndexedPosition implementation

// Class method to create an instance with a given integer index.
+ (instancetype)positionWithIndex:(NSUInteger)index
{
    APLIndexedPosition *indexedPosition = [[self alloc] init];
    indexedPosition.index = index;
    return indexedPosition;
}

@end

@implementation APLIndexedRange

// Class method to create an instance with a given range
+ (instancetype)indexedRangeWithRange:(NSRange)range
{
    if (range.location == NSNotFound) {
        return nil;
    }
    
    APLIndexedRange *indexedRange = [[self alloc] init];
    indexedRange.range = range;
    return indexedRange;
}


// UITextRange read-only property - returns start index of range.
- (UITextPosition *)start
{
    return [APLIndexedPosition positionWithIndex:self.range.location];
}


// UITextRange read-only property - returns end index of range.
- (UITextPosition *)end
{
    return [APLIndexedPosition positionWithIndex:(self.range.location + self.range.length)];
}


// UITextRange read-only property - returns YES if range is zero length.
-(BOOL)isEmpty
{
    return (self.range.length == 0);
}

@end
