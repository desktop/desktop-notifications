import QuickLRU from 'quick-lru'
import { v4 as uuidv4 } from 'uuid'
import * as os from 'os'

export function supportsNotifications() {
  if (process.platform !== 'win32') {
    return false
  }

  const versionComponents = os.release().split('.')
  const majorVersion = parseInt(versionComponents[0], 10)

  // Only Windows 10 and newer are supported
  return majorVersion >= 10
}

// Windows-only for now
const nativeModule = supportsNotifications()
  ? require('../../build/Release/desktop-notifications.node')
  : null

export function initializeNotifications(toastActivatorClsid: string) {
  nativeModule?.initializeNotifications(
    toastActivatorClsid,
    onNotificationEvent
  )
}

export function terminateNotifications() {
  nativeModule?.terminateNotifications()
}

function showNotification(
  id: string,
  title: string,
  body: string
): number | null {
  return nativeModule?.showNotification(id, title, body) ?? null
}

function closeNotification(id: string) {
  if (!nativeModule) {
    // this code is a no-op when the module is missing
    return
  }

  return nativeModule.closeNotification(id)
}

type DesktopNotificationEvent = 'click'

const shownNotifications = new QuickLRU<string, DesktopNotification>({
  maxSize: 200,
})

function onNotificationEvent(event: DesktopNotificationEvent, id: string) {
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

export class DesktopNotification {
  public onclick?: () => void
  private readonly id: string

  constructor(public readonly title: string, public readonly body: string) {
    this.id = uuidv4()
  }

  public show() {
    shownNotifications.set(this.id, this)
    showNotification(this.id, this.title, this.body)
  }

  public close() {
    shownNotifications.delete(this.id)
    closeNotification(this.id)
  }
}
