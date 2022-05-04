import { supportsNotifications } from './notification-support'

export function getNotificationSettingsUrl() {
  if (!supportsNotifications()) {
    return null
  }

  return process.platform === 'darwin'
    ? 'x-apple.systempreferences:com.apple.preference.notifications'
    : 'ms-settings:notifications'
}
