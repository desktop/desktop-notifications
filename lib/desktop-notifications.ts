import QuickLRU from 'quick-lru'
import { v4 as uuidv4 } from 'uuid'
import * as os from 'os'

export function supportsNotifications() {
  if (process.platform === 'darwin') {
    return true
  }

  if (process.platform !== 'win32') {
    return false
  }

  const versionComponents = os.release().split('.')
  const majorVersion = parseInt(versionComponents[0], 10)

  // Only Windows 10 and newer are supported
  return majorVersion >= 10
}

// The native binary will be loaded lazily to avoid any possible crash at start
// time, which are harder to trace.
let _nativeModule: any | null | undefined = undefined

function getNativeModule() {
  if (_nativeModule !== undefined) {
    return _nativeModule
  }

  _nativeModule = supportsNotifications()
    ? require('../../build/Release/desktop-notifications.node')
    : null

  return _nativeModule
}

/**
 * Initializes the desktop-notifications system.
 *
 * @param toastActivatorClsid CLSID used by Windows to report notification events
 */
export function initializeNotifications(toastActivatorClsid: string) {
  getNativeModule()?.initializeNotifications(
    toastActivatorClsid,
    onNotificationEvent
  )
}

/** Terminates the desktop-notifications system. */
export function terminateNotifications() {
  getNativeModule()?.terminateNotifications()
}

type DesktopNotificationPermission = 'default' | 'granted' | 'denied'

export function getNotificationsPermission(): DesktopNotificationPermission {
  return getNativeModule()?.getNotificationsPermission()
}

function showNotification(
  id: string,
  title: string,
  body: string
): number | null {
  return getNativeModule()?.showNotification(id, title, body) ?? null
}

function closeNotification(id: string) {
  return getNativeModule()?.closeNotification(id)
}

type DesktopNotificationEvent = 'click'

const shownNotifications = new QuickLRU<string, DesktopNotification>({
  maxSize: 200,
})

function onNotificationEvent(
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) {
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
  /** Handler of click events. */
  public onclick?: () => void

  private readonly id: string

  constructor(public readonly title: string, public readonly body: string) {
    this.id = uuidv4()
  }

  /** Shows the notification. */
  public show() {
    shownNotifications.set(this.id, this)
    showNotification(this.id, this.title, this.body)
  }

  /** Closes the notification if it was ever shown. */
  public close() {
    shownNotifications.delete(this.id)
    closeNotification(this.id)
  }
}
