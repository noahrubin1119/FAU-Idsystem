function doGet(e){
  Logger.log("--- doGet ---");
 
 var tag = "",
     enter = "",
     id = "",
     magstrip = 0,
     barcode = 0,
     Fname = "",
     Lname = "",
     studentID = 0,
     email = "",
     birthday = "";
 
  try {
    if (e == null){e={}; e.parameters = {id:"", item1:"test", item2:"1"};}
    
    id = e.parameters.id;
    
    if(id == "Att"){
      tag = e.parameters.item1;
      enter = e.parameters.item2;
      // save the data to spreadsheet
      save_Att_data(tag, enter);
      //this is the message that comes when you submit the data 
      return ContentService.createTextOutput("Wrote:\n  tag: " + tag + "\n  enter_data: " + enter + "\n id: " +id);

    }
    else if(id == "Reg"){
      magstrip = e.parameters.item1;
      barcode = e.parameters.item2;
      Fname = e.parameters.item3;
      Lname = e.parameters.item4;
      studentID = e.parameters.item5;
      email = e.parameters.item6;
      birthday = e.parameters.item7;
      
      // save the data to spreadsheet
      save_Reg_data(magstrip, barcode, Fname, Lname, studentID, email, birthday);
      //this is the message that comes when you submit the data
      return ContentService.createTextOutput("Wrote:\n  barcode: " + barcode + "\n  magstrip: " + magstrip+ "\n Fname: " + Fname + "\n Lname: " + Lname + "\n studentID: "+ studentID + "\n email: " + email + "\n birthday: " + birthday +"\n id: " +id);
    }
    
   
 
  } catch(error) { 
    Logger.log(error);    
    return ContentService.createTextOutput("oops...." + error.message 
                                            + "\n" + new Date() 
                                            + "\ntag: " + tag +
                                            + "\nenter_data: " + enter);
  }  
}
 
// Method to save given data to a sheet
function save_Att_data(tag, enter){
  Logger.log("--- save_Att_data ---"); 
 
 
  try {
    
    //makes the date and time vars for the logging
    var dateTime = new Date();
    var time = dateTime.getHours() + ":" + dateTime.getMinutes() + ":" + dateTime.getSeconds();
    
    var ss = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/1aG8YUy_LUKO0R_qw4Djk3e1fkeQeyX5lnIbdLfBdvkQ/edit");
    var dataLoggerSheet = ss.getSheetByName("Sheet1");
 
    // Get last edited row from Sheet1 sheet
    var row = dataLoggerSheet.getLastRow() + 1;
    
    
    if(enter == 1){ //a 1 represents an entry
      dataLoggerSheet.getRange("D" + row).setValue("Entry");
    }
    else if(enter == 2){ //autofill
      Logger.log(enter);
      dataLoggerSheet.getRange("E" + row).setValue("Autofilled");
      var status = lastOccuranceAtt(tag);
      dataLoggerSheet.getRange("D" + row).setValue(status);
    }
    else{ //anything else is an exit
      dataLoggerSheet.getRange("D" + row).setValue("Exit");
    }
    
    // Start Populating the data
    dataLoggerSheet.getRange("A" + row).setValue(tag); // ID
    dataLoggerSheet.getRange("B" + row).setValue(dateTime); // dateTime
    
    dataLoggerSheet.getRange("C" + row).setValue(time); //time
    
    
  }
 
  catch(error) {
    Logger.log(JSON.stringify(error));
  }
 
  Logger.log("--- save_Att_data end---"); 
}

function lastOccuranceAtt(tag){
  var ss = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/1aG8YUy_LUKO0R_qw4Djk3e1fkeQeyX5lnIbdLfBdvkQ/edit");
  var dataLoggerSheet = ss.getSheetByName("Sheet1");
  var data = dataLoggerSheet.getDataRange().getValues();
  
  var lastOccuranceAtt = 0;
  for(var i = 0; i<data.length;i++){
    if(data[i][0] == tag){ //[0] because column A shows the person's id
      if(lastOccuranceAtt < i){
        lastOccuranceAtt = i;
      }
    }
  }
  
  var status = "Entry";
    if(data[lastOccuranceAtt][3] == "Exit"){
      status = "Entry"
    }
    else if(data[lastOccuranceAtt][3] == "Entry"){
      status = "Exit"
    }
  return status;
}

function save_Reg_data(magstrip, barcode, Fname, Lname, studentID, email, birthday){
  Logger.log("--- save_Reg_data ---"); 
  Logger.log("magstrip: " + magstrip + "   barcode: " + barcode+ "\n Fname: " + Fname +"\n Lname: " + Lname + "\n studentID: "+ studentID + "\n email: " + email + "\n birthday: " + birthday);

 
  try {
    
    //makes the date and time vars for the logging
    Logger.log("trying");
    var dateTime = new Date();
    var time = dateTime.getHours() + ":" + dateTime.getMinutes() + ":" + dateTime.getSeconds();
    Logger.log("time done");
    var ss = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/1aG8YUy_LUKO0R_qw4Djk3e1fkeQeyX5lnIbdLfBdvkQ/edit");
    var dataLoggerSheet = ss.getSheetByName("InputtedUsers");
    Logger.log("connected to sheet");
    Logger.log("magstrip: " + magstrip + "   barcode: " + barcode+ "\n Fname: " + Fname +"\n Lname: " + Lname + "\n studentID: "+ studentID + "\n email: " + email + "\n birthday: " + birthday);
    var row = dataLoggerSheet.getLastRow() + 1;
    Logger.log("row: " + row);
    
    dataLoggerSheet.getRange("H" + row).setValue(magstrip); // Linking!!!
    dataLoggerSheet.getRange("G" + row).setValue(barcode); // Linking!!!
    dataLoggerSheet.getRange("B" + row).setValue(Fname); // Linking!!!
    dataLoggerSheet.getRange("C" + row).setValue(Lname); // Linking!!!
    dataLoggerSheet.getRange("D" + row).setValue(email); // Linking!!!
    dataLoggerSheet.getRange("E" + row).setValue(studentID); // Linking!!!
    dataLoggerSheet.getRange("F" + row).setValue(birthday); // Linking!!!
    
    var dateTimeCombo = dateTime + " " + time;
    dataLoggerSheet.getRange("A" + row).setValue(dateTimeCombo);
  }
 
  catch(error) {
    Logger.log("ERROR: " + JSON.stringify(error));
  }
 
  Logger.log("--- save_Reg_data end---"); 
}
