import { ipcRenderer } from 'electron'

setupClickEventListener('showNotificationButton', showNotification)
setupClickEventListener('checkPermissionsButton', checkPermissions)

function setupClickEventListener(id: string, onclick: () => void) {
  const element = document.getElementById(id)
  if (element === null) {
    throw new Error(`Could not find notification with id ${id}`)
  }
  element.addEventListener('click', onclick)
}

function showNotification() {
  ipcRenderer.send('show-notification')
}

async function checkPermissions() {
  console.log(
    'checkPermissions',
    await ipcRenderer.invoke('get-notifications-permission')
  )
}
