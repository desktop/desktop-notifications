#import "GHDesktopNotificationsManager.h"

#import <UserNotifications/UserNotifications.h>

@interface GHDesktopNotificationsManager () <UNUserNotificationCenterDelegate>

@end

@implementation GHDesktopNotificationsManager

- (instancetype)init
{
  if ((self = [super init]))
  {
    [self configureNotificationsDelegate];
  }
  return self;
}

- (void)configureNotificationsDelegate
{
  UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
  center.delegate = self;
}

#pragma mark - UNUserNotificationCenterDelegate methods

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
didReceiveNotificationResponse:(UNNotificationResponse *)response
         withCompletionHandler:(void (^)(void))completionHandler
{
  NSLog(@"didReceive 1");
  if (self.completionHandler == nil)
  {
    NSLog(@"didReceive 2");
    completionHandler();
    return;
  }

  UNNotificationRequest *request = response.notification.request;
  NSLog(@"didReceive 3: %@", request.content.userInfo);
  self.completionHandler(@"click", request.identifier, request.content.userInfo, completionHandler);
  NSLog(@"didReceive 4");
}

@end
