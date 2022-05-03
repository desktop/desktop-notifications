import { v4 as uuidv4 } from 'uuid'
import QuickLRU from 'quick-lru'
import { closeNotification, showNotification } from './native-module'

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
    showNotification(this.id, this.title, this.body, this.userInfo)
  }

  /** Closes the notification if it was ever shown. */
  public close() {
    shownNotifications.delete(this.id)
    closeNotification(this.id)
  }
}
