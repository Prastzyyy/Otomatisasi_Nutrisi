//Program ESP_Spreadsheet Otomatisasi Nutrisi

/*
function doGet(e) { 
  Logger.log(JSON.stringify(e));
  var result = 'Ok';
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1IKQU8h4dVo9KKv69Chr1sNzfcQOmlh4LeF0Fe2pe_4k';   // Spreadsheet ID.
    var sheet_name = "Data_Otomatisasi_Nutrisi";  // Sheet Name in Google Sheets.

    var sheet_open = SpreadsheetApp.openById(sheet_id);
    var sheet_target = sheet_open.getSheetByName(sheet_name);

    var newRow = sheet_target.getLastRow() + 1;

    var rowDataLog = [];

    var Data_for_I3;
    var Data_for_J3;
    var Data_for_K3;
    var Data_for_L3;
    var Data_for_M3;
    var Data_for_N3;
    var Data_for_O3;

    var Curr_Date = Utilities.formatDate(new Date(), "Asia/Jakarta", 'dd/MM/yyyy');
    rowDataLog[0] = Curr_Date;  // Date will be written in column A (in the "DHT11 Sensor Data Logger" section).
    Data_for_I3 = Curr_Date;  // Date will be written in column I3 (in the "Latest DHT11 Sensor Data" section).

    var Curr_Time = Utilities.formatDate(new Date(), "Asia/Jakarta", 'HH:mm:ss');
    rowDataLog[1] = Curr_Time;  // Time will be written in column B (in the "DHT11 Sensor Data Logger" section).
    Data_for_J3 = Curr_Time;  // Time will be written in column J3 (in the "Latest DHT11 Sensor Data" section).

    var sts_val = '';

    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'sts':
          sts_val = value;
          break;

        case 'pH':
          rowDataLog[2] = value;  // Sensor Reading Status will be written in column C (in the "DHT11 Sensor Data Logger" section).
          Data_for_K3 = value;  // Sensor Reading Status will be written in column K3 (in the "Latest DHT11 Sensor Data" section).
          result += ', pH Written on column C';
          break;

        case 'PPM':
          rowDataLog[3] = value;  // The temperature value will be written in column D (in the "DHT11 Sensor Data Logger" section).
          Data_for_L3 = value;  // The temperature value will be written in column L3 (in the "Latest DHT11 Sensor Data" section).
          result += ', PPM Written on column D';
          break;

        case 'tinggi_cm':
          rowDataLog[4] = value; // The humidity value will be written in column E (in the "DHT11 Sensor Data Logger" section).
          Data_for_M3 = value;  // The humidity value will be written in column M3 (in the "Latest DHT11 Sensor Data" section).
          result += ', Ketinggian Air Written on column E';
          break;

        case 'suhu':
          rowDataLog[5] = value;  // The state of Switch_1 will be written in column F (in the "DHT11 Sensor Data Logger" section).
          Data_for_N3 = value;  // The state of Switch_1 will be written in column N3 (in the "Latest DHT11 Sensor Data" section).
          result += ', Suhu Written on column F';
          break; 

          case 'status':
          rowDataLog[6] = value;  // The state of Switch_1 will be written in column F (in the "DHT11 Sensor Data Logger" section).
          Data_for_O3 = value;  // The state of Switch_1 will be written in column N3 (in the "Latest DHT11 Sensor Data" section).
          result += ', Status Written on column G';
          break; 

        default:
          result += ", unsupported parameter";
      }
    }
    
    // Conditions for writing data received from ESP32 to Google Sheets.
    if (sts_val == 'write') {
      // Writes data to the "DHT11 Sensor Data Logger" section.
      Logger.log(JSON.stringify(rowDataLog));
      var newRangeDataLog = sheet_target.getRange(newRow, 1, 1, rowDataLog.length);
      newRangeDataLog.setValues([rowDataLog]);
      
      // Write the data to the "Latest DHT11 Sensor Data" section.
      var RangeDataLatest = sheet_target.getRange('I3:O3');
      RangeDataLatest.setValues([[Data_for_I3, Data_for_J3, Data_for_K3, Data_for_L3, Data_for_M3, Data_for_N3, Data_for_O3]]);

      return ContentService.createTextOutput(result);
    }
    
    // Conditions for sending data to ESP32 when ESP32 reads data from Google Sheets.
    if (sts_val == 'read') {
      // Use the line of code below if you want ESP32 to read data from columns I3 to O3 (Date,Time,Sensor Reading Status,Temperature,Humidity,Switch 1, Switch 2).
      // var all_Data = sheet_target.getRange('I3:O3').getDisplayValues();
      
      // Use the line of code below if you want ESP32 to read data from columns K3 to O3 (Sensor Reading Status,Temperature,Humidity,Switch 1, Switch 2).
      var all_Data = sheet_target.getRange('K3:O3').getValues();
      return ContentService.createTextOutput(all_Data);
    }
  }
}
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}
 
 */
