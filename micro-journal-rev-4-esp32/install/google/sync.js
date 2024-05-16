// PUT the FOLDER PATH OF YOUR LIKING
// FOLDER SHOULD EXIST SO CREATE ONE BEFORE SYNCING
const _FOLDER_PATH = "/uJournal";

function doPost(e) {
  try {
    //
    var fileContent = e.postData.contents;

    fileContent = Utilities.base64Decode(fileContent);
    fileContent = Utilities.newBlob(fileContent).getDataAsString("ISO-8859-1"); // decode with extended ascii
    fileContent = Utilities.newBlob(fileContent).getBytes(); // to byte array
    fileContent = Utilities.newBlob(fileContent).getDataAsString("UTF-8"); // encode to utf-8

    //
    var fileName = getFormattedDate() + "_uJournal.txt";

    // Create file in Google Drive
    var folder = getFolder(_FOLDER_PATH);
    var file = folder.createFile(fileName, fileContent, "text/plain; charset=utf-8");

    let jsonResponse = ContentService.createTextOutput(
      JSON.stringify({ status: "OK" })
    );
    jsonResponse.setMimeType(ContentService.MimeType.JSON);

    return jsonResponse;

    ///
  } catch (error) {
    // If an exception occurs, send an email notification
    sendErrorEmail(error, e);

    // Return error response
    let errorResponse = ContentService.createTextOutput(
      JSON.stringify({ status: "ERROR", message: "An error occurred" })
    );
    errorResponse.setMimeType(ContentService.MimeType.JSON);

    return errorResponse;
  }
}

function sendErrorEmail(error, e) {
  var recipient = Session.getEffectiveUser().getEmail(); // Change this to your email address
  var subject = "Error in Micro Journal Sync Process";
  var body = error.stack + '\n\n' + JSON.stringify(e);

  MailApp.sendEmail(recipient, subject, body);
}

function getFormattedDate() {
  var now = new Date();
  var year = now.getFullYear();
  var month = padZero(now.getMonth() + 1);
  var day = padZero(now.getDate());
  var hours = padZero(now.getHours());
  var minutes = padZero(now.getMinutes());
  return year + "." + month + "." + day + "-" + hours + "." + minutes;
}

function padZero(num) {
  return (num < 10 ? "0" : "") + num;
}

function getFolder(folderPath) {
  var folders = folderPath.split("/");
  var folder = DriveApp.getRootFolder();
  for (var i = 0; i < folders.length; i++) {
    if (folders[i] !== "") {
      var subFolders = folder.getFoldersByName(folders[i]);
      if (subFolders.hasNext()) {
        folder = subFolders.next();
      } else {
        return null;
      }
    }
  }
  return folder;
}
