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

export function supportsNotificationsPermissionRequest() {
  return process.platform === 'darwin'
}
