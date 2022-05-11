#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^GHNotificationCompletionHandler)(
  NSString *event,
  NSString *identifier,
  NSDictionary *userInfo,
  dispatch_block_t completionHandler
);

@interface GHDesktopNotificationsManager : NSObject

@property (nonatomic, nullable) GHNotificationCompletionHandler completionHandler;

@end

NS_ASSUME_NONNULL_END
