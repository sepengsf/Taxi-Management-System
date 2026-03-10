object Dmod: TDmod
  Height = 1080
  Width = 1440
  PixelsPerInch = 144
  object ADOConnection1: TADOConnection
    ConnectionString = 
      'Provider=Microsoft.ACE.OLEDB.12.0;Data Source=C:\Users\User\Docu' +
      'ments\Embarcadero\Studio\Projects\new new project\app\TaxiDB.acc' +
      'db;Persist Security Info=False'
    LoginPrompt = False
    Provider = 'Microsoft.ACE.OLEDB.12.0'
    Left = 145
    Top = 176
  end
  object ADODataSet1: TADODataSet
    Connection = ADOConnection1
    Parameters = <>
    Left = 96
    Top = 424
  end
  object ADOQuery1: TADOQuery
    Connection = ADOConnection1
    Parameters = <>
    Left = 304
    Top = 200
  end
  object DataSource1: TDataSource
    DataSet = ADOQuery1
    Left = 200
    Top = 64
  end
  object ADOQuery2: TADOQuery
    Parameters = <>
    Left = 1056
    Top = 240
  end
  object ADOQuery3: TADOQuery
    Parameters = <>
    Left = 704
    Top = 200
  end
  object DataSource2: TDataSource
    Left = 912
    Top = 144
  end
end
