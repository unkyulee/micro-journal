function doPost(e) {
  var fileContent = e.postData.contents;
  var fileName = "uJournal_" + getFormattedDate() + ".txt";

  // Create file in Google Drive
  var folder = DriveApp.getRootFolder();
  var file = folder.createFile(fileName, fileContent);


  let jsonResponse = ContentService.createTextOutput(JSON.stringify({ status: "OK" }));
  jsonResponse.setMimeType(ContentService.MimeType.JSON);

  return jsonResponse;
}

function getFormattedDate() {
  var now = new Date();
  var year = now.getFullYear();
  var month = padZero(now.getMonth() + 1);
  var day = padZero(now.getDate());
  var hours = padZero(now.getHours());
  var minutes = padZero(now.getMinutes());
  return year + "." + month + "." + day + "." + hours + "." + minutes;
}

function padZero(num) {
  return (num < 10 ? "0" : "") + num;
}