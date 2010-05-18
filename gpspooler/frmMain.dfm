object frmGPSpooler: TfrmGPSpooler
  Left = 419
  Top = 189
  Width = 674
  Height = 423
  Caption = 'GPSpooler'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 666
    Height = 389
    ActivePage = TabSheet2
    Align = alClient
    TabIndex = 1
    TabOrder = 0
    object tshJobs: TTabSheet
      Caption = 'Jobs'
    end
    object TabSheet2: TTabSheet
      Caption = 'Settings'
      ImageIndex = 1
      object Printers: TGroupBox
        Left = 0
        Top = 8
        Width = 249
        Height = 217
        Caption = 'Printers'
        TabOrder = 0
        object lbPrinterList: TListBox
          Left = 2
          Top = 41
          Width = 245
          Height = 174
          Align = alClient
          ItemHeight = 13
          TabOrder = 0
        end
        object Panel1: TPanel
          Left = 2
          Top = 15
          Width = 245
          Height = 26
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 1
          object btnRefresh: TButton
            Left = 0
            Top = 0
            Width = 75
            Height = 25
            Caption = 'Refresh'
            TabOrder = 0
            OnClick = btnRefreshClick
          end
        end
      end
      object GroupBox1: TGroupBox
        Left = 264
        Top = 16
        Width = 185
        Height = 105
        Caption = 'Environment Variables'
        TabOrder = 1
        object btnSetEnv: TButton
          Left = 6
          Top = 16
          Width = 75
          Height = 25
          Caption = 'Install'
          TabOrder = 0
          OnClick = btnSetEnvClick
        end
        object rbCurrentUser: TRadioButton
          Left = 16
          Top = 48
          Width = 113
          Height = 17
          Caption = 'For Current User'
          TabOrder = 1
        end
        object rbSystem: TRadioButton
          Left = 16
          Top = 72
          Width = 113
          Height = 17
          Caption = 'For All'
          Checked = True
          TabOrder = 2
          TabStop = True
        end
        object btnDelEnv: TButton
          Left = 88
          Top = 16
          Width = 75
          Height = 25
          Caption = 'Uninstall'
          TabOrder = 3
          OnClick = btnDelEnvClick
        end
      end
    end
  end
  object TrayIcon1: TTrayIcon
    Visible = True
    Hide = True
    RestoreOn = imDoubleClick
    PopupMenuOn = imNone
    Left = 616
    Top = 344
  end
end
