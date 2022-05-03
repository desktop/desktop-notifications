import { DesktopNotificationEvent } from './notification-event-type'
import { shownNotifications } from './notification'

export type NotificationCallback = (
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) => void

let globalCallback: NotificationCallback | null = null

export const onNotificationEvent: NotificationCallback = (
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) => {
  console.log(`Notification event: ${event} ${id}`, userInfo)

  const notification = shownNotifications.get(id)
  if (notification === undefined) {
    // Events of notifications that are not in the cache will be handled via the
    // global callback. We can safely assume those notifications were posted
    // in a different app session.
    globalCallback?.(event, id, userInfo)
    return
  }

  if (event === 'click') {
    notification.onclick?.()
  }
  // TODO: handle other events?
}

export const setGlobalCallback = (callback: NotificationCallback | null) => {
  globalCallback = callback
}
