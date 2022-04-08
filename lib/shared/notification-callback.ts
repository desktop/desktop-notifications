import { DesktopNotificationEvent } from './notification-event-type'
import { shownNotifications } from './notification'

export type NotificationCallback = (
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) => void

export const onNotificationEvent: NotificationCallback = (
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) => {
  console.log(`Notification event: ${event} ${id}`, userInfo)

  const notification = shownNotifications.get(id)
  if (notification === undefined) {
    // TODO: handle notifications that are not in the cache
    return
  }

  if (event === 'click') {
    notification.onclick?.()
  }
  // TODO: handle other events?
}
