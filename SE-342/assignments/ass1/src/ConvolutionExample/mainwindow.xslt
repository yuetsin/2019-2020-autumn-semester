<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>602</width>
    <height>430</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>MainWindow</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <property name="sizePolicy">
    <sizepolicy hsizetype="Expanding" vsizetype="Expanding">
     <horstretch>0</horstretch>
     <verstretch>0</verstretch>
    </sizepolicy>
   </property>
   <layout class="QHBoxLayout" name="horizontalLayout_2">
    <item>
     <layout class="QHBoxLayout" name="horizontalLayout">
      <item>
       <widget class="QGroupBox" name="groupBox">
        <property name="sizePolicy">
         <sizepolicy hsizetype="Expanding" vsizetype="Expanding">
          <horstretch>0</horstretch>
          <verstretch>0</verstretch>
         </sizepolicy>
        </property>
        <property name="title">
         <string>預覽</string>
        </property>
        <property name="alignment">
         <set>Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter</set>
        </property>
        <layout class="QVBoxLayout" name="verticalLayout_2">
         <item>
          <widget class="QGraphicsView" name="graphicsView"/>
         </item>
         <item>
          <widget class="QSlider" name="horizontalSlider">
           <property name="maximum">
            <number>100</number>
           </property>
           <property name="value">
            <number>50</number>
           </property>
           <property name="orientation">
            <enum>Qt::Horizontal</enum>
           </property>
          </widget>
         </item>
        </layout>
       </widget>
      </item>
     </layout>
    </item>
    <item>
     <layout class="QVBoxLayout" name="verticalLayout">
      <item>
       <widget class="QGroupBox" name="groupBox_3">
        <property name="sizePolicy">
         <sizepolicy hsizetype="Preferred" vsizetype="Expanding">
          <horstretch>0</horstretch>
          <verstretch>0</verstretch>
         </sizepolicy>
        </property>
        <property name="title">
         <string>變數設定</string>
        </property>
        <layout class="QVBoxLayout" name="verticalLayout_4">
         <item>
          <layout class="QHBoxLayout" name="horizontalLayout_3">
           <item>
            <widget class="QLabel" name="label">
             <property name="sizePolicy">
              <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
               <horstretch>0</horstretch>
               <verstretch>0</verstretch>
              </sizepolicy>
             </property>
             <property name="text">
              <string>卷積方式</string>
             </property>
            </widget>
           </item>
           <item>
            <widget class="QComboBox" name="comboBox">
             <item>
              <property name="text">
               <string>Roberts 卷積</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Prewitt 卷積</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Sobel 卷積</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Gaussian 濾波</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Mean 濾波</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Median 濾波</string>
              </property>
             </item>
            </widget>
           </item>
          </layout>
         </item>
         <item>
          <layout class="QHBoxLayout" name="horizontalLayout_5">
           <item>
            <widget class="QLabel" name="label_3">
             <property name="sizePolicy">
              <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
               <horstretch>0</horstretch>
               <verstretch>0</verstretch>
              </sizepolicy>
             </property>
             <property name="text">
              <string>卷積核大小:</string>
             </property>
            </widget>
           </item>
           <item>
            <widget class="QLabel" name="kernelSizeLabel">
             <property name="text">
              <string>3</string>
             </property>
            </widget>
           </item>
           <item>
            <widget class="QSlider" name="kernelSizeSlider">
             <property name="minimum">
              <number>3</number>
             </property>
             <property name="maximum">
              <number>13</number>
             </property>
             <property name="singleStep">
              <number>2</number>
             </property>
             <property name="pageStep">
              <number>3</number>
             </property>
             <property name="value">
              <number>5</number>
             </property>
             <property name="orientation">
              <enum>Qt::Horizontal</enum>
             </property>
            </widget>
           </item>
          </layout>
         </item>
         <item>
          <layout class="QHBoxLayout" name="horizontalLayout_4">
           <item>
            <widget class="QLabel" name="label_2">
             <property name="sizePolicy">
              <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
               <horstretch>0</horstretch>
               <verstretch>0</verstretch>
              </sizepolicy>
             </property>
             <property name="text">
              <string>Sigma:</string>
             </property>
            </widget>
           </item>
           <item>
            <widget class="QLabel" name="sigmaSizeLabel">
             <property name="text">
              <string>50</string>
             </property>
            </widget>
           </item>
           <item>
            <widget class="QSlider" name="sigmaSizeSlider">
             <property name="minimum">
              <number>1</number>
             </property>
             <property name="maximum">
              <number>100</number>
             </property>
             <property name="value">
              <number>50</number>
             </property>
             <property name="orientation">
              <enum>Qt::Horizontal</enum>
             </property>
            </widget>
           </item>
          </layout>
         </item>
        </layout>
       </widget>
      </item>
      <item>
       <widget class="QGroupBox" name="groupBox_2">
        <property name="sizePolicy">
         <sizepolicy hsizetype="Preferred" vsizetype="Fixed">
          <horstretch>0</horstretch>
          <verstretch>0</verstretch>
         </sizepolicy>
        </property>
        <property name="title">
         <string>控制鈕</string>
        </property>
        <layout class="QVBoxLayout" name="verticalLayout_3">
         <item>
          <widget class="QPushButton" name="pushButton_2">
           <property name="text">
            <string>應用卷積</string>
           </property>
          </widget>
         </item>
         <item>
          <widget class="QPushButton" name="pushButton">
           <property name="enabled">
            <bool>false</bool>
           </property>
           <property name="text">
            <string>應用濾波</string>
           </property>
          </widget>
         </item>
         <item>
          <widget class="QPushButton" name="pushButton_3">
           <property name="text">
            <string>重置</string>
           </property>
          </widget>
         </item>
        </layout>
       </widget>
      </item>
     </layout>
    </item>
   </layout>
  </widget>
  <widget class="QMenuBar" name="menubar">
   <property name="geometry">
    <rect>
     <x>0</x>
     <y>0</y>
     <width>602</width>
     <height>22</height>
    </rect>
   </property>
   <widget class="QMenu" name="menuMain_Window">
    <property name="title">
     <string>檔案</string>
    </property>
    <addaction name="actionImportImage"/>
    <addaction name="actionExportImage"/>
    <addaction name="separator"/>
    <addaction name="actionResume"/>
   </widget>
   <widget class="QMenu" name="menu">
    <property name="title">
     <string>進階</string>
    </property>
   </widget>
   <widget class="QMenu" name="menu_2">
    <property name="title">
     <string>說明</string>
    </property>
    <addaction name="actionShowAbout"/>
   </widget>
   <widget class="QMenu" name="menu_3">
    <property name="title">
     <string>顯示</string>
    </property>
    <addaction name="actionZoomIn"/>
    <addaction name="actionZoomOut"/>
    <addaction name="separator"/>
    <addaction name="actionReset"/>
   </widget>
   <addaction name="menuMain_Window"/>
   <addaction name="menu_3"/>
   <addaction name="menu"/>
   <addaction name="menu_2"/>
  </widget>
  <widget class="QStatusBar" name="statusbar"/>
  <action name="actionImportImage">
   <property name="text">
    <string>匯入圖片檔</string>
   </property>
   <property name="shortcut">
    <string>Ctrl+O</string>
   </property>
  </action>
  <action name="actionExportImage">
   <property name="text">
    <string>匯出圖片檔</string>
   </property>
   <property name="toolTip">
    <string>匯入圖片檔</string>
   </property>
   <property name="shortcut">
    <string>Ctrl+S</string>
   </property>
  </action>
  <action name="actionResume">
   <property name="text">
    <string>恢復默認</string>
   </property>
   <property name="shortcut">
    <string>Ctrl+R</string>
   </property>
  </action>
  <action name="actionZoomIn">
   <property name="text">
    <string>放大一點</string>
   </property>
   <property name="toolTip">
    <string>放大一點</string>
   </property>
   <property name="shortcut">
    <string>Ctrl+=</string>
   </property>
  </action>
  <action name="actionZoomOut">
   <property name="text">
    <string>縮小一點</string>
   </property>
   <property name="toolTip">
    <string>縮小一點</string>
   </property>
   <property name="shortcut">
    <string>Ctrl+-</string>
   </property>
  </action>
  <action name="actionReset">
   <property name="text">
    <string>實際大小</string>
   </property>
   <property name="toolTip">
    <string>實際大小</string>
   </property>
   <property name="shortcut">
    <string>Ctrl+0</string>
   </property>
  </action>
  <action name="actionShowAbout">
   <property name="text">
    <string>App 版本</string>
   </property>
   <property name="toolTip">
    <string>App 版本</string>
   </property>
  </action>
 </widget>
 <tabstops>
  <tabstop>graphicsView</tabstop>
  <tabstop>horizontalSlider</tabstop>
 </tabstops>
 <resources/>
 <connections>
  <connection>
   <sender>kernelSizeSlider</sender>
   <signal>sliderMoved(int)</signal>
   <receiver>kernelSizeLabel</receiver>
   <slot>setNum(int)</slot>
   <hints>
    <hint type="sourcelabel">
     <x>496</x>
     <y>154</y>
    </hint>
    <hint type="destinationlabel">
     <x>410</x>
     <y>155</y>
    </hint>
   </hints>
  </connection>
  <connection>
   <sender>sigmaSizeSlider</sender>
   <signal>sliderMoved(int)</signal>
   <receiver>sigmaSizeLabel</receiver>
   <slot>setNum(int)</slot>
   <hints>
    <hint type="sourcelabel">
     <x>486</x>
     <y>217</y>
    </hint>
    <hint type="destinationlabel">
     <x>386</x>
     <y>217</y>
    </hint>
   </hints>
  </connection>
 </connections>
</ui>
