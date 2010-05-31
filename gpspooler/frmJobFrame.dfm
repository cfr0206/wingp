object frmJobs: TfrmJobs
  Left = 0
  Top = 0
  Width = 449
  Height = 65
  AutoSize = True
  Color = clMoneyGreen
  ParentColor = False
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 449
    Height = 65
    Align = alTop
    BevelInner = bvLowered
    ParentColor = True
    TabOrder = 0
    object Panel2: TPanel
      Left = 248
      Top = 2
      Width = 199
      Height = 61
      Align = alRight
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
      object btnSendToPrinter: TBitBtn
        Left = 136
        Top = 8
        Width = 60
        Height = 25
        Caption = 'Print'
        TabOrder = 0
        OnClick = btnSendToPrinterClick
      end
      object BitBtn2: TBitBtn
        Left = 72
        Top = 8
        Width = 60
        Height = 25
        Caption = 'Cancel'
        TabOrder = 1
      end
      object BitBtn1: TBitBtn
        Left = 8
        Top = 8
        Width = 60
        Height = 25
        Caption = 'Delete'
        TabOrder = 2
      end
    end
    object Panel3: TPanel
      Left = 2
      Top = 2
      Width = 246
      Height = 61
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 1
      object lbJobName: TLabel
        Left = 0
        Top = 0
        Width = 246
        Height = 13
        Align = alTop
        Caption = 'lbJobName'
      end
      object lbPrinterName: TLabel
        Left = 0
        Top = 13
        Width = 246
        Height = 13
        Align = alTop
        Caption = 'lbPrinterName'
      end
      object edCopies: TLabeledEdit
        Left = 36
        Top = 32
        Width = 65
        Height = 21
        EditLabel.Width = 31
        EditLabel.Height = 13
        EditLabel.Caption = 'copies'
        LabelPosition = lpLeft
        LabelSpacing = 3
        TabOrder = 0
        Text = '1'
        OnKeyPress = edCopiesKeyPress
      end
      object udCopies: TUpDown
        Left = 101
        Top = 32
        Width = 17
        Height = 21
        Associate = edCopies
        Min = 1
        Max = 10000
        Position = 1
        TabOrder = 1
        Wrap = False
      end
      object chbDelAfterPrinting: TCheckBox
        Left = 128
        Top = 32
        Width = 121
        Height = 17
        Caption = 'Delete afte printing'
        Checked = True
        State = cbChecked
        TabOrder = 2
      end
    end
  end
end
