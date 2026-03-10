//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit3.h"
#include "Unit1.h"
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
	: TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TForm3::FormCreate(TObject *Sender)
{
	lblCompany->Caption = "CINDY TAXI RANK";
	lblCompany->Font->Size = 30;
    lblCompany->Font->Color = clRed;

	lstDisplay->Items->Clear();

	lstDisplay->Items->Add("Developed by: Sindisiwe F Sepeng");
	lstDisplay->Items->Add("Student No.: 212179361");
	lstDisplay->Items->Add("Course: TPG201T - TAXI MANAGEMENT SYSTEM");

    sgQueueLog->ColCount = 5;
    sgQueueLog->RowCount = 1;
    sgQueueLog->Cells[0][0] = "Driver";
    sgQueueLog->Cells[1][0] = "Rank";
    sgQueueLog->Cells[2][0] = "Position";
    sgQueueLog->Cells[3][0] = "Time In";
    sgQueueLog->Cells[4][0] = "Waiting Time";

    // Trip Log headers
    sgTripLog->ColCount = 5;
    sgTripLog->RowCount = 1;
    sgTripLog->Cells[0][0] = "Driver";
    sgTripLog->Cells[1][0] = "Rank";
    sgTripLog->Cells[2][0] = "Trip Time";
    sgTripLog->Cells[3][0] = "Est Time";
    sgTripLog->Cells[4][0] = "Vehicle";

    lblRecordCount->Caption = "Records: 0";
}
//---------------------------------------------------------------------------

//Load queue_log.txt and trip_assignment_log.txt 
void __fastcall TForm3::btnLoadDataClick(TObject *Sender)
{
 TStringList *list = new TStringList;

    list->LoadFromFile("queue_log.txt");

    sgQueueLog->RowCount = 1;   //header
    int queueRow = 1;

    for (int i = 1; i < list->Count; i++) 
    {
        TStringList *row = new TStringList;
        row->Delimiter = ',';
        row->DelimitedText = list->Strings[i];

        sgQueueLog->RowCount++;

        sgQueueLog->Cells[0][queueRow] = row->Strings[3]; 
        sgQueueLog->Cells[1][queueRow] = row->Strings[2]; 
        sgQueueLog->Cells[2][queueRow] = IntToStr(queueRow);  
        sgQueueLog->Cells[3][queueRow] = row->Strings[1]; 

        sgQueueLog->Cells[4][qRow] =
            IntToStr(queueRow * 5) + " min";

        queueRow++;
        delete row;
    }

    list->Clear();
    list->LoadFromFile("trip_assignment_log.txt");

    sgTripLog->RowCount = 1;
    int tripRow = 1;

    for (int i = 1; i < list->Count; i++) 
    {
        TStringList *row = new TStringList;
        row->Delimiter = ',';
        row->DelimitedText = list->Strings[i];

        sgTripLog->RowCount++;
      
        sgTripLog->Cells[0][tripRow] = row->Strings[2]; 
        sgTripLog->Cells[1][tripRow] = row->Strings[3]; 
        sgTripLog->Cells[2][tripRow] = row->Strings[4]; 
        sgTripLog->Cells[3][tripRow] = row->Strings[1]; 

        sgTripLog->Cells[4][tripRow] =
            IntToStr(Random(15) + 10) + " min";

		tripRow++;
		delete row;
	}

	lblRecordCount->Caption =
		"Records: " + IntToStr(queueRow - 1);

	delete list;

}
//---------------------------------------------------------------------------
void __fastcall TForm3::btnApplyFilterClick(TObject *Sender)
{

	String filterVal = InputBox("Filter", "Enter value:", "");
	int filterCol = (rgFilter->ItemIndex == 0) ? 1 : 0;

	int count = 0;

	for(int i = 1; i < sgQueueLog->RowCount; i++)
	{
		if(sgQueueLog->Cells[filterCol][i] == filterVal)
		{
			sgQueueLog->RowHeights[i] = 24;
			count++;
		}
		else
		{
			sgQueueLog->RowHeights[i] = 0; 
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::btnConnectClick(TObject *Sender)
{

String path = ExtractFilePath(Application->ExeName) + "TaxiDB.accdb";

  Dmod->ADOConnection1->ConnectionString =
	"Provider=Microsoft.ACE.OLEDB.12.0;"
	"Data Source=" + path;
	Dmod->ADOConnection1->Connected = false;
	  Dmod->ADOConnection1->LoginPrompt = false;
	   Dmod->ADOConnection1->ConnectionString =
			"Provider=Microsoft.ACE.OLEDB.12.0;"
			"Data Source=" + path + ";"
			"Persist Security Info=False;";

	try
	{


		Dmod->ADOConnection1->Connected = true;
        		
			Dmod->ADOQuery1->Connection = Dmod->ADOConnection1;
			Dmod->ADOQuery2->Connection = Dmod->ADOConnection1;
			Dmod->ADOQuery3->Connection = 	Dmod->ADOConnection1;
							 
if (SchedulesEmpty())
{
	PopulateSchedules();
}
		Dmod->DataSource2->DataSet = Dmod->ADOQuery2;
	DBGridTrips->DataSource   = Dmod->DataSource2;

		lblStatus->Caption = "Status: Connected";
		ShowMessage("Database connected successfully");
        LoadFareGrids();
        LoadRoutes();


			Dmod->DataSource1->DataSet = 	Dmod->ADOQuery1;
			DBGrid1->DataSource = 	Dmod->DataSource1;

			
			Dmod->ADOQuery1->Close();
			Dmod->ADOQuery1->SQL->Clear();
			Dmod->ADOQuery1->SQL->Add("SELECT D.DriverName, D.DriverID, V.Model, V.VehicleID, S.MaxPassengers ");
			Dmod->ADOQuery1->SQL->Add("FROM (Drivers AS D ");
			Dmod->ADOQuery1->SQL->Add("LEFT JOIN Schedules AS S ON D.DriverID = S.DriverID) ");
			Dmod->ADOQuery1->SQL->Add("LEFT JOIN Vehicles AS V ON S.VehicleID = V.VehicleID");

			Dmod->ADOQuery1->Open();
	}
	catch (Exception &e)
	{
		lblStatus->Caption = "Status: Disconnected";
		ShowMessage("Connection failed: " + e.Message);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::LoadQueueLog(std::vector<TDriverInfo> &drivers)
{
	drivers.clear();

	TStringList *list = new TStringList;
	try
	{
		list->LoadFromFile("queue_log.txt");

		for (int i = 0; i < list->Count; i++)
		{
			TStringList *row = new TStringList;
			row->Delimiter = ',';
			row->DelimitedText = list->Strings[i];

			if (row->Count >= 2)
			{
				TDriverInfo info;
				info.Driver = row->Strings[0].Trim();
				info.Rank   = row->Strings[1].Trim();
				drivers.push_back(info);
			}
			delete row;
		}
	}
	__finally
	{
		delete list;
	}
}




void __fastcall TForm3::btnSimulateTripsClick(TObject *Sender)
{

	if (!Dmod->ADOConnection1->Connected)
	{
		ShowMessage("Connect to database");
		return;
	}

	Dmod->ADOQuery2->Connection = Dmod->ADOConnection1;
	Dmod->ADOQuery3->Connection = Dmod->ADOConnection1;

	Randomize();

	for (int i = 0; i < 10; i++)
	{
	
		Dmod->ADOQuery2->Close();
		Dmod->ADOQuery2->SQL->Text =
			"SELECT TOP 1 ScheduleID FROM Schedules ORDER BY Rnd()";
		Dmod->ADOQuery2->Open();

		if (Dmod->ADOQuery2->Eof)
			return;

		int scheduleID =
			Dmod->ADOQuery2->FieldByName("ScheduleID")->AsInteger;

		int passengers = Random(15) + 1;
		TDateTime tripDate = Now() - Random(7);

		Dmod->ADOQuery3->Close();
		Dmod->ADOQuery3->SQL->Text =
			"INSERT INTO Trips (TripDate, ScheduleID, PassengersBoarded) "
			"VALUES (:tdate, :schedule, :pas)";

		Dmod->ADOQuery3->Parameters->ParamByName("tdate")->Value = tripDate;
		Dmod->ADOQuery3->Parameters->ParamByName("schedule")->Value = scheduleID;
		Dmod->ADOQuery3->Parameters->ParamByName("pas")->Value = passengers;

		Dmod->ADOQuery3->ExecSQL();
	}

	ShowMessage("10 trips simulated successfully");
	RefreshTripsGrid();


}
//---------------------------------------------------------------------------
void __fastcall TForm3::Button1Click(TObject *Sender)
{

	if (!Dmod->ADOConnection1->Connected)
	{
		ShowMessage("Connect to database");
		return;
	}

	Dmod->ADOQuery3->Connection = Dmod->ADOConnection1;

	RichEditReport->Clear();
	RichEditReport->Lines->Add("TAXI TRIP REPORT");
	RichEditReport->Lines->Add("==============================");

	Dmod->ADOQuery3->Close();
	Dmod->ADOQuery3->SQL->Clear();
	Dmod->ADOQuery3->SQL->Add(
		"SELECT T.TripDate, T.PassengersBoarded, "
		"D.DriverName, V.Model "
		"FROM (((Trips AS T "
		"INNER JOIN Schedules AS S ON T.ScheduleID = S.ScheduleID) "
		"INNER JOIN Drivers AS D ON S.DriverID = D.DriverID) "
		"INNER JOIN Vehicles AS V ON S.VehicleID = V.VehicleID) "
		"ORDER BY T.TripDate DESC"
	);

	Dmod->ADOQuery3->Open();

	while (!Dmod->ADOQuery3->Eof)
	{
		RichEditReport->Lines->Add(
			DateTimeToStr(Dmod->ADOQuery3->FieldByName("TripDate")->AsDateTime) +
			" | " +
			Dmod->ADOQuery3->FieldByName("DriverName")->AsString +
			" | " +
			Dmod->ADOQuery3->FieldByName("Model")->AsString +
			" | Passengers: " +
			Dmod->ADOQuery3->FieldByName("PassengersBoarded")->AsString
		);

		RichEditReport->Lines->Add("----------------------------------");
		Dmod->ADOQuery3->Next();
	}

}
//---------------------------------------------------------------------------
void __fastcall TForm3::btnSaveReportClick(TObject *Sender)
{
	if (SaveDialog1->Execute())
		RichEditReport->Lines->SaveToFile(SaveDialog1->FileName);
}
//---------------------------------------------------------------------------
void TForm3::LoadRoutes()
{


	if (!Dmod->ADOConnection1->Connected)
		return;

	cmbRoutes->Clear();

	Dmod->ADOQuery3->Close();
	Dmod->ADOQuery3->Connection = Dmod->ADOConnection1;
	Dmod->ADOQuery3->SQL->Clear();
	Dmod->ADOQuery3->SQL->Text =
		"SELECT RankFrom, RankTo, DistanceKm FROM Routes";

	Dmod->ADOQuery3->Open();

	while (!Dmod->ADOQuery3->Eof)
	{
		String text =
			Dmod->ADOQuery3->FieldByName("RankFrom")->AsString +
			" → " +
			Dmod->ADOQuery3->FieldByName("RankTo")->AsString +
			" (" +
			Dmod->ADOQuery3->FieldByName("DistanceKm")->AsString +
			" km)";

		int distance =
			Dmod->ADOQuery3->FieldByName("DistanceKm")->AsInteger;

		cmbRoutes->Items->AddObject(text, (TObject*)distance);

		Dmod->ADOQuery3->Next();
	}

}

void TForm3::LoadFareGrids()
{
	if (!Dmod->ADOConnection1->Connected)
		return;

	// ================= RANK GRID =================
	sgRanks->ColCount = 2;
	sgRanks->RowCount = 1;

	sgRanks->Cells[0][0] = "From Rank";
	sgRanks->Cells[1][0] = "To Rank";

	// ================= ROUTE GRID =================
	sgRoutes->ColCount = 2;
	sgRoutes->RowCount = 1;

	sgRoutes->Cells[0][0] = "Distance (km)";
	sgRoutes->Cells[1][0] = "Fare (R)";

	// Load Routes from DB
	TADOQuery *query = new TADOQuery(NULL);
	try
	{
		query->Connection = Dmod->ADOConnection1;
		query->SQL->Text =
			"SELECT RankFrom, RankTo, DistanceKm FROM Routes";
		query->Open();

		int row = 1;
		while (!query->Eof)
		{
			sgRanks->RowCount++;
			sgRoutes->RowCount++;

			sgRanks->Cells[0][row] =
				query->FieldByName("RankFrom")->AsString;
			sgRanks->Cells[1][row] =
				query->FieldByName("RankTo")->AsString;

			int dist = query->FieldByName("DistanceKm")->AsInteger;
			double fare = dist * 10.0;

			sgRoutes->Cells[0][row] = IntToStr(dist);
			sgRoutes->Cells[1][row] =
				FormatFloat("0.00", fare);

			row++;
			query->Next();
		}
	}
	__finally
	{
		delete query;
	}
}

void __fastcall TForm3::btnCalcFareClick(TObject *Sender)
{

	if (cmbRoutes->ItemIndex < 0)
	{
		ShowMessage("Choose a route");
		return;
	}

	int distance =
		(int)cmbRoutes->Items->Objects[cmbRoutes->ItemIndex];

	double baseFare = distance * 10.0;
	double discount = baseFare * 0.1;
	double vat = (baseFare - discount) * 0.15;
	double total = baseFare - discount + vat;

	RichEditTicket->Clear();
	RichEditTicket->Lines->Add("TAXI TICKET");
	RichEditTicket->Lines->Add("------------------");
	RichEditTicket->Lines->Add("Distance: " + IntToStr(distance) + " km");
	RichEditTicket->Lines->Add("Base Fare: R" + FormatFloat("0.00", baseFare));
	RichEditTicket->Lines->Add("Discount: R" + FormatFloat("0.00", discount));
	RichEditTicket->Lines->Add("VAT: R" + FormatFloat("0.00", vat));
	RichEditTicket->Lines->Add("TOTAL: R" + FormatFloat("0.00", total));


}
//---------------------------------------------------------------------------
void __fastcall TForm3::btnSaveTicketClick(TObject *Sender)
{
  if (SaveDialog2->Execute())
		RichEditTicket->Lines->SaveToFile(SaveDialog2->FileName);
}
//---------------------------------------------------------------------------

void TForm3::PopulateSchedules()
{

	if (!Dmod->ADOConnection1->Connected)
		return;

	TADOQuery *q = new TADOQuery(NULL);
	try
	{
		query->Connection = Dmod->ADOConnection1;

		query->SQL->Text = "DELETE FROM Trips";
		query->ExecSQL();

		query->SQL->Text = "DELETE FROM Schedules";
		query->ExecSQL();

		query->SQL->Text = "SELECT DriverID FROM Drivers";
		query->Open();

		while (!q->Eof)
		{
			int driverID = query->FieldByName("DriverID")->AsInteger;
			int vehicleID = Random(10) + 1;
			int maxPassengers = 15;

			TADOQuery *insert = new TADOQuery(NULL);
			insert->Connection = Dmod->ADOConnection1;
			insert->SQL->Text =
				"INSERT INTO Schedules (DriverID, VehicleID, MaxPassengers) "
				"VALUES (" +
				IntToStr(driverID) + ", " +
				IntToStr(vehicleID) + ", " +
				IntToStr(maxPassengers) + ")";
			ins->ExecSQL();
			delete insert;

			query->Next();
		}
	}
	__finally
	{
		delete query;
	}



}


  void TForm3::RefreshTripsGrid() {

	if (!Dmod->ADOConnection1->Connected)
		return;

	Dmod->ADOQuery2->Close();
	Dmod->ADOQuery2->Connection = Dmod->ADOConnection1;
	Dmod->ADOQuery2->SQL->Text =
		"SELECT * FROM Trips ORDER BY TripID DESC";

	Dmod->ADOQuery2->Open();

    Dmod->DataSource2->DataSet = Dmod->ADOQuery2;
	DBGridTrips->DataSource = Dmod->DataSource2;

}
// Checks if Schedules table has data
bool TForm3::SchedulesEmpty()
{
    Dmod->ADOQuery3->Close();
    Dmod->ADOQuery3->SQL->Text = "SELECT COUNT(*) AS C FROM Schedules";
    Dmod->ADOQuery3->Open();

    return Dmod->ADOQuery3->FieldByName("C")->AsInteger == 0;
}


void __fastcall TForm3::btnDeleteTripsClick(TObject *Sender)
{
	if (!Dmod->ADOConnection1->Connected)
	{
		ShowMessage("Connect to database");
		return;
	}

	if (DBGridTrips->DataSource->DataSet->IsEmpty())
	{
		ShowMessage("No trip selected");
		return;
	}

	int tripID =
		DBGridTrips->DataSource->DataSet
		->FieldByName("TripID")->AsInteger;

	Dmod->ADOQuery3->Close();
	Dmod->ADOQuery3->SQL->Clear();
	Dmod->ADOQuery3->SQL->Text =
		"DELETE FROM Trips WHERE TripID = :id";

	Dmod->ADOQuery3->Parameters
		->ParamByName("id")->Value = tripID;

	Dmod->ADOQuery3->ExecSQL();

	ShowMessage("Trip deleted successfully");

	RefreshTripsGrid(); // refresh grid
}

//---------------------------------------------------------------------------

