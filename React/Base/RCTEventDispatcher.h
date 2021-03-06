/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#import <AppKit/AppKit.h>

#import "RCTBridge.h"

typedef NS_ENUM(NSInteger, RCTTextEventType)
{
  RCTTextEventTypeFocus,
  RCTTextEventTypeBlur,
  RCTTextEventTypeChange,
  RCTTextEventTypeSubmit,
  RCTTextEventTypeEnd,
  RCTTextEventTypeKeyPress
};

/**
 * The threshold at which text inputs will start warning that the JS thread
 * has fallen behind (resulting in poor input performance, missed keys, etc.)
 */
RCT_EXTERN const NSInteger RCTTextUpdateLagWarningThreshold;

/**
 * Takes an input event name and normalizes it to the form that is required
 * by the events system (currently that means starting with the "top" prefix,
 * but that's an implementation detail that may change in future).
 */
RCT_EXTERN NSString *RCTNormalizeInputEventName(NSString *eventName);

@protocol RCTEvent <NSObject>
@required

@property (nonatomic, strong, readonly) NSNumber *viewTag;
@property (nonatomic, copy, readonly) NSString *eventName;
@property (nonatomic, assign, readonly) uint16_t coalescingKey;

- (BOOL)canCoalesce;
- (id<RCTEvent>)coalesceWithEvent:(id<RCTEvent>)newEvent;

// used directly for doing a JS call
+ (NSString *)moduleDotMethod;
// must contain only JSON compatible values
- (NSArray *)arguments;

@end


/**
 * This class wraps the -[RCTBridge enqueueJSCall:args:] method, and
 * provides some convenience methods for generating event calls.
 */
@interface RCTEventDispatcher : NSObject <RCTBridgeModule>

/**
 * Deprecated, do not use.
 */
- (void)sendAppEventWithName:(NSString *)name body:(id)body
__deprecated_msg("Subclass RCTEventEmitter instead");

/**
 * Deprecated, do not use.
 */
- (void)sendDeviceEventWithName:(NSString *)name body:(id)body
__deprecated_msg("Subclass RCTEventEmitter instead");

/**
 * Deprecated, do not use.
 */
- (void)sendInputEventWithName:(NSString *)name body:(NSDictionary *)body
__deprecated_msg("Use RCTDirectEventBlock or RCTBubblingEventBlock instead");

/**
 * Send a text input/focus event. For internal use only.
 */
- (void)sendTextEventWithType:(RCTTextEventType)type
                     reactTag:(NSNumber *)reactTag
                         text:(NSString *)text
                          key:(NSString *)key
                   eventCount:(NSInteger)eventCount;

/**
 * Send a pre-prepared event object.
 *
 * If the event can be coalesced it is added to a pool of events that are sent at the beginning of the next js frame.
 * Otherwise if the event cannot be coalesced we first flush the pool of coalesced events and the new event after that.
 *
 * Why it works this way?
 * Making sure js gets events in the right order is crucial for correctly interpreting gestures.
 * Unfortunately we cannot emit all events as they come. If we would do that we would have to emit scroll and touch moved event on every frame,
 * which is too much data to transfer and process on older devices. This is especially bad when js starts lagging behind main thread.
 */
- (void)sendEvent:(id<RCTEvent>)event;

@end

@interface RCTBridge (RCTEventDispatcher)

- (RCTEventDispatcher *)eventDispatcher;

@end
