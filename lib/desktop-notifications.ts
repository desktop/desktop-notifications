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

  nativeModule.initializeNotifications(toastActivatorClsid)
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
  body: string,
  callback: (event: string) => void
): number | null {
  if (!nativeModule) {
    // this code is a no-op when the module is missing
    return null
  }

  return nativeModule.showNotification(id, title, body, callback)
}

function closeNotification(id: string) {
  if (!nativeModule) {
    // this code is a no-op when the module is missing
    return
  }

  return nativeModule.closeNotification(id)
}

export class DesktopNotification {
  public onclick?: () => void
  private readonly id: string

  constructor(public readonly title: string, public readonly body: string) {
    this.id = uuidv4()
  }

  public show() {
    showNotification(this.id, this.title, this.body, (event: string) => {
      if (event === 'click') {
        this.onclick?.()
      }
    })
  }

  public close() {
    closeNotification(this.id)
  }
}
