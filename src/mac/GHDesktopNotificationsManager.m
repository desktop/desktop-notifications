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
  if (self.completionHandler == nil)
  {
    completionHandler();
    return;
  }

  UNNotificationRequest *request = response.notification.request;
  self.completionHandler(@"click", request.identifier, request.content.userInfo, completionHandler);
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler
{
  // This will make sure the notification is displayed even when the app is focused
  completionHandler(UNNotificationPresentationOptionAlert
                    | UNNotificationPresentationOptionBadge
                    | UNNotificationPresentationOptionSound);
}

@end
