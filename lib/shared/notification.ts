import { v4 as uuidv4 } from 'uuid'
import QuickLRU from 'quick-lru'
import * as win32 from '../win32'
import * as darwin from '../darwin'

const platformModule = process.platform === 'win32' ? win32 : darwin

export const shownNotifications = new QuickLRU<string, DesktopNotification>({
  maxSize: 200,
})

export class DesktopNotification {
  /** Handler of click events. */
  public onclick?: () => void

  private readonly id: string

  constructor(
    public readonly title: string,
    public readonly body: string,
    public readonly userInfo?: Record<string, any>
  ) {
    this.id = uuidv4()
  }

  /** Shows the notification. */
  public show() {
    shownNotifications.set(this.id, this)
    platformModule.showNotification(
      this.id,
      this.title,
      this.body,
      this.userInfo
    )
  }

  /** Closes the notification if it was ever shown. */
  public close() {
    shownNotifications.delete(this.id)
    platformModule.closeNotification(this.id)
  }
}
