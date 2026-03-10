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
	lblCompany->Caption = "RUBBERMAN TAXI RANK";
	lblCompany->Font->Size = 28;
    lblCompany->Font->Color = clNavy;


	lstDisplay->Items->Clear();

	lstDisplay->Items->Add("Developer: MUZI Thami NKOSI");
	lstDisplay->Items->Add("Student Number: 212002321");
	lstDisplay->Items->Add("Course: TPG201T");
	lstDisplay->Items->Add("Project: Taxi Management System");



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

//Loads queue_log.txt and trip_assignment_log.txt into StringGrids
void __fastcall TForm3::btnLoadDataClick(TObject *Sender)
{
 TStringList *list = new TStringList;

    // ================= QUEUE LOG =================
    list->LoadFromFile("queue_log.txt");

    sgQueueLog->RowCount = 1;   // keep header
    int qRow = 1;

    for (int i = 1; i < list->Count; i++) // skip header
    {
        TStringList *row = new TStringList;
        row->Delimiter = ',';
        row->DelimitedText = list->Strings[i];

        sgQueueLog->RowCount++;

        // File format:
        // Date, Time, RankID, DriverID
        sgQueueLog->Cells[0][qRow] = row->Strings[3]; // Driver
        sgQueueLog->Cells[1][qRow] = row->Strings[2]; // Rank
        sgQueueLog->Cells[2][qRow] = IntToStr(qRow);  // Position
        sgQueueLog->Cells[3][qRow] = row->Strings[1]; // Time In

        // Waiting Time = position * 5 minutes
        sgQueueLog->Cells[4][qRow] =
            IntToStr(qRow * 5) + " min";

        qRow++;
        delete row;
    }

    // ================= TRIP ASSIGNMENT LOG =================
    list->Clear();
    list->LoadFromFile("trip_assignment_log.txt");

    sgTripLog->RowCount = 1;
    int tRow = 1;

    for (int i = 1; i < list->Count; i++) // skip header
    {
        TStringList *row = new TStringList;
        row->Delimiter = ',';
        row->DelimitedText = list->Strings[i];

        sgTripLog->RowCount++;

        // File format:
        // Date, Time, DriverID, RankFrom, RankTo
        sgTripLog->Cells[0][tRow] = row->Strings[2]; // Driver
        sgTripLog->Cells[1][tRow] = row->Strings[3]; // From Rank
        sgTripLog->Cells[2][tRow] = row->Strings[4]; // To Rank
        sgTripLog->Cells[3][tRow] = row->Strings[1]; // Trip Time

        // Estimated Time (random realistic value)
        sgTripLog->Cells[4][tRow] =
            IntToStr(Random(15) + 10) + " min";

		tRow++;
		delete row;
	}

	// ================= RECORD COUNT =================
	lblRecordCount->Caption =
		"Records: " + IntToStr(qRow - 1);

	delete list;

}
//---------------------------------------------------------------------------
//  Filters queue log by Driver or Rank& hide rows does not match
void __fastcall TForm3::btnApplyFilterClick(TObject *Sender)
{

	String filterValue = InputBox("Filter", "Enter value:", "");
	int filterCol = (rgFilter->ItemIndex == 0) ? 1 : 0;

	int count = 0;

	for(int i = 1; i < sgQueueLog->RowCount; i++)
	{
		if(sgQueueLog->Cells[filterCol][i] == filterValue)
		{
			sgQueueLog->RowHeights[i] = 24;
			count++;
		}
		else
		{
			sgQueueLog->RowHeights[i] = 0; // hide row
		}
	}
}
//---------------------------------------------------------------------------
// Purpose : Connects application to TaxiDB.accdb database
void __fastcall TForm3::btnConnectClick(TObject *Sender)
{

String dbPath = ExtractFilePath(Application->ExeName) + "TaxiDB.accdb";

  Dmod->ADOConnection1->ConnectionString =
	"Provider=Microsoft.ACE.OLEDB.12.0;"
	"Data Source=" + dbPath;
	Dmod->ADOConnection1->Connected = false;
	  Dmod->ADOConnection1->LoginPrompt = false;
	   Dmod->ADOConnection1->ConnectionString =
			"Provider=Microsoft.ACE.OLEDB.12.0;"
			"Data Source=" + dbPath + ";"
			"Persist Security Info=False;";

	try
	{


		Dmod->ADOConnection1->Connected = true;
        		// 3. Link the Grid components
			Dmod->ADOQuery1->Connection = Dmod->ADOConnection1;
			Dmod->ADOQuery2->Connection = Dmod->ADOConnection1;
			Dmod->ADOQuery3->Connection = 	Dmod->ADOConnection1;
							 // 🔄 Populate schedules ONLY if empty
if (SchedulesEmpty())
{
	PopulateSchedules();
}
		Dmod->DataSource2->DataSet = Dmod->ADOQuery2;
	DBGridTrips->DataSource   = Dmod->DataSource2;

		lblStatus->Caption = "Status: Connected";
		ShowMessage("Database connected successfully");
        LoadFareGrids();
		// Populate schedules ONCE after connecting
		   LoadRoutes();





			Dmod->DataSource1->DataSet = 	Dmod->ADOQuery1;
			DBGrid1->DataSource = 	Dmod->DataSource1;

			// 4. The Complex SQL Join (Drivers + Schedules + Vehicles)
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
		ShowMessage("Connect to database first");
		return;
	}

	// FORCE connections
	Dmod->ADOQuery2->Connection = Dmod->ADOConnection1;
	Dmod->ADOQuery3->Connection = Dmod->ADOConnection1;

	Randomize();

	for (int i = 0; i < 10; i++)
	{
		// Get random ScheduleID
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

		// Insert trip
		Dmod->ADOQuery3->Close();
		Dmod->ADOQuery3->SQL->Text =
			"INSERT INTO Trips (TripDate, ScheduleID, PassengersBoarded) "
			"VALUES (:dt, :sid, :pax)";

		Dmod->ADOQuery3->Parameters->ParamByName("dt")->Value = tripDate;
		Dmod->ADOQuery3->Parameters->ParamByName("sid")->Value = scheduleID;
		Dmod->ADOQuery3->Parameters->ParamByName("pax")->Value = passengers;

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
		ShowMessage("Connect to database first");
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
//  Generates taxi trip report

void __fastcall TForm3::btnSaveReportClick(TObject *Sender)
{
	if (SaveDialog1->Execute())
		RichEditReport->Lines->SaveToFile(SaveDialog1->FileName);
}
//---------------------------------------------------------------------------
//Loads routes from database into combo box
void TForm3::LoadRoutes()
{


	if (!Dmod->ADOConnection1->Connected)
		return;

	cmbRoutes->Clear();

	Dmod->ADOQuery3->Close();
	Dmod->ADOQuery3->Connection = Dmod->ADOConnection1; // IMPORTANT
	Dmod->ADOQuery3->SQL->Clear();
	Dmod->ADOQuery3->SQL->Text =
		"SELECT RankFrom, RankTo, DistanceKm FROM Routes";

	Dmod->ADOQuery3->Open();

	while (!Dmod->ADOQuery3->Eof)
	{
		String routeText =
			Dmod->ADOQuery3->FieldByName("RankFrom")->AsString +
			" → " +
			Dmod->ADOQuery3->FieldByName("RankTo")->AsString +
			" (" +
			Dmod->ADOQuery3->FieldByName("DistanceKm")->AsString +
			" km)";

		int distance =
			Dmod->ADOQuery3->FieldByName("DistanceKm")->AsInteger;

		cmbRoutes->Items->AddObject(routeText, (TObject*)distance);

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
	TADOQuery *q = new TADOQuery(NULL);
	try
	{
		q->Connection = Dmod->ADOConnection1;
		q->SQL->Text =
			"SELECT RankFrom, RankTo, DistanceKm FROM Routes";
		q->Open();

		int row = 1;
		while (!q->Eof)
		{
			sgRanks->RowCount++;
			sgRoutes->RowCount++;

			// Rank grid
			sgRanks->Cells[0][row] =
				q->FieldByName("RankFrom")->AsString;
			sgRanks->Cells[1][row] =
				q->FieldByName("RankTo")->AsString;

			// Route grid
			int dist = q->FieldByName("DistanceKm")->AsInteger;
			double fare = dist * 10.0;

			sgRoutes->Cells[0][row] = IntToStr(dist);
			sgRoutes->Cells[1][row] =
				FormatFloat("0.00", fare);

			row++;
			q->Next();
		}
	}
	__finally
	{
		delete q;
	}
}

 //   - Calculates base fare,Applies discount and Displays ticket details
void __fastcall TForm3::btnCalcFareClick(TObject *Sender)
{

	if (cmbRoutes->ItemIndex < 0)
	{
		ShowMessage("Please select a route");
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

 //Saves fare ticket to file
void __fastcall TForm3::btnSaveTicketClick(TObject *Sender)
{
  if (SaveDialog2->Execute())
		RichEditTicket->Lines->SaveToFile(SaveDialog2->FileName);
}
//---------------------------------------------------------------------------

	 // Automatically creates schedules for each driver
void TForm3::PopulateSchedules()
{

	if (!Dmod->ADOConnection1->Connected)
		return;

	TADOQuery *q = new TADOQuery(NULL);
	try
	{
		q->Connection = Dmod->ADOConnection1;

		// 1️⃣ DELETE TRIPS FIRST (FK dependency)
		q->SQL->Text = "DELETE FROM Trips";
		q->ExecSQL();

		// 2️⃣ DELETE SCHEDULES
		q->SQL->Text = "DELETE FROM Schedules";
		q->ExecSQL();

		// 3️⃣ GET DRIVERS
		q->SQL->Text = "SELECT DriverID FROM Drivers";
		q->Open();

		while (!q->Eof)
		{
			int driverID = q->FieldByName("DriverID")->AsInteger;
			int vehicleID = Random(10) + 1;
			int maxPassengers = 15;

			TADOQuery *ins = new TADOQuery(NULL);
			ins->Connection = Dmod->ADOConnection1;
			ins->SQL->Text =
				"INSERT INTO Schedules (DriverID, VehicleID, MaxPassengers) "
				"VALUES (" +
				IntToStr(driverID) + ", " +
				IntToStr(vehicleID) + ", " +
				IntToStr(maxPassengers) + ")";
			ins->ExecSQL();
			delete ins;

			q->Next();
		}
	}
	__finally
	{
		delete q;
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
		ShowMessage("Connect to database first");
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

