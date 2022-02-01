import { v4 as uuidv4 } from 'uuid'

const nativeModule =
  process.platform === 'win32'
    ? require('../../build/Release/desktop-notifications.node')
    : null

export function initializeNotifications(toastActivatorClsid: string) {
  if (!nativeModule) {
    // this code is a no-op when the module is missing
    return
  }

  nativeModule.initializeNotifications(toastActivatorClsid, onNotificationEvent)
}

export function terminateNotifications() {
  if (!nativeModule) {
    // this code is a no-op when the module is missing
    return
  }

  nativeModule.terminateNotifications()
}

function showNotification(
  id: string,
  title: string,
  body: string
): number | null {
  if (!nativeModule) {
    // this code is a no-op when the module is missing
    return null
  }

  return nativeModule.showNotification(id, title, body)
}

function closeNotification(id: string) {
  if (!nativeModule) {
    // this code is a no-op when the module is missing
    return
  }

  return nativeModule.closeNotification(id)
}

type DesktopNotificationEvent = 'click'

const shownNotifications: Map<string, DesktopNotification> = new Map()

function onNotificationEvent(event: DesktopNotificationEvent, id: string) {
  const notification = shownNotifications.get(id)
  if (notification === undefined) {
    return
  }

  if (event === 'click') {
    notification.onclick?.()
  }
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
