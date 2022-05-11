import * as os from 'os'

/**
 * Whether or not the current system supports these notifications. As of today,
 * only macOS and Windows 10+ are supported.
 */
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

/**
 * Whether or not the current system supports asking the user for permission to
 * display notifications. As of today, only macOS supports this.
 */
export function supportsNotificationsPermissionRequest() {
  return process.platform === 'darwin'
}
