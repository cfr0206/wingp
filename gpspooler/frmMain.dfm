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
    ActivePage = tshJobs
    Align = alClient
    TabIndex = 0
    TabOrder = 0
    object tshJobs: TTabSheet
      Caption = 'Jobs'
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 658
        Height = 41
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
      end
      object pnlJobs: TScrollBox
        Left = 0
        Top = 41
        Width = 658
        Height = 320
        VertScrollBar.Smooth = True
        Align = alClient
        TabOrder = 1
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Settings'
      ImageIndex = 1
      object GroupBox1: TGroupBox
        Left = 0
        Top = 0
        Width = 658
        Height = 129
        Align = alTop
        Caption = 'Environment Variables'
        TabOrder = 0
        object Panel3: TPanel
          Left = 2
          Top = 15
          Width = 114
          Height = 112
          Align = alLeft
          AutoSize = True
          TabOrder = 0
          object btnSetEnv: TButton
            Left = 1
            Top = 3
            Width = 104
            Height = 25
            Caption = 'Install'
            TabOrder = 0
            OnClick = btnSetEnvClick
          end
          object btnDelEnv: TButton
            Left = 1
            Top = 29
            Width = 104
            Height = 25
            Caption = 'Uninstall'
            TabOrder = 1
            OnClick = btnDelEnvClick
          end
          object rbSystem: TRadioButton
            Left = 7
            Top = 64
            Width = 73
            Height = 17
            Caption = 'For All'
            Checked = True
            TabOrder = 2
            TabStop = True
          end
          object rbCurrentUser: TRadioButton
            Left = 7
            Top = 80
            Width = 106
            Height = 17
            Caption = 'For Current User'
            TabOrder = 3
          end
        end
        object mmEnv: TMemo
          Left = 116
          Top = 15
          Width = 540
          Height = 112
          Align = alClient
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          ReadOnly = True
          ScrollBars = ssBoth
          TabOrder = 1
          WordWrap = False
        end
      end
    end
  end
  object TrayIcon1: TTrayIcon
    Visible = True
    Hide = True
    RestoreOn = imDoubleClick
    PopupMenuOn = imNone
    Left = 608
    Top = 328
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 536
    Top = 328
  end
end
