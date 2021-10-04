
import socket
import csv
import pandas as pd
from datetime import datetime
from time import sleep
import tkinter as tk 
from tkinter import ttk
import tkinter.messagebox
from tkinter import* 
import os
from tkinter.filedialog import askopenfilename
import numpy as np
from pandasgui import show

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

TCP_IP = "192.168.***.***"
TCP_PORT = **
LISTM = b"list"
file_path = "C:/Cumulus/data/Python/" ##enter your path

###################################################################
def bSaveFile ( fil, data):
   if data =='':
      print ('Nema podataka!!!')
   else:
      flpath = file_path + fil
      text_file = open( flpath, "w")
      n = text_file.write(data)
      text_file.close()
   return n
###################################################################
def bSaveML():
   global fldata
   fl = filechoosen.get()
   print (fl)
   print (fldata)
   nchar =  saveFile( fl, fldata)
   if nchar > 0: print ('File saved in ',fl,' , ', nchar, ' characters')

###################################################################
def bLoadML():
   global fldata
   fl = filechoosen.get()
   fldata = loadFileML(fl)
   Tt.insert(END, fldata)

###################################################################

def loadFileML(fn):
   sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   sock.connect((TCP_IP, TCP_PORT))
   sleep(0.1)
   my_string =''
   data = b''
   myBuffer = b''
   MESSAGE = ("read /" + fn).encode('utf-8')
   print("message: %s" % MESSAGE)
   sock.send( MESSAGE )
   myBuffer = sock.recv(1)
   myBuffer = sock.recv(3)
   #print (myBuffer)
   #print (str(myBuffer, encoding='utf-8'))
   size = 0
   for i in range(3):
      size += myBuffer[i] << ((2-i)*8)
   print( 'File lenth = ', size)

   myBuffer = b""
   current_size = 0
   while current_size < size:
      data = sock.recv(1024)
      #print('New chunk ')
      #print (str(data, encoding='utf-8'))
      if not data:
         break
      if len(data) + current_size > size:
         data = data[:size-current_size] # trim additional data
      myBuffer += data
      # you can stream here to disk
      current_size += len(data)
   my_string = str(myBuffer, encoding='utf-8')
   sock.close()
   buttSaveML.config(state="normal")
   return (my_string)
###########################################################################
def bLoadFL():
   global fldata
   file_path = askopenfilename(initialdir = "C:/Cumulus/data/Python/",title = "Select file",filetypes = (("txt files","*.txt"),("all files","*.*")))
   try:
      with open(file_path) as f:
         fldata = f.read()
   except IOError as e:
      print("Couldn't open or write to file (%s)." % e)
      messagebox.showerror("Error", "Couldn't open file " )
      return
   fldata = fldata.replace("\n\n","\n")
   Ttm.insert(END, fldata)
   buttProcDT.config(state="normal")
###########################################################################
def bProcDT():
   global df_num
   global df_monthly
   df_monthly = obrada_monthlytable(fldata)
   Ttm.delete(1.0,END)
   Ttm.insert(END, to_print_df (df_monthly))
   sleep(2)
   df_with_null = df_monthly[df_monthly.isna().any(axis=1)]

   if len(df_with_null.index.values) != 0:
      Ttm.delete(1.0,END)
      Ttm.insert(END,  to_print_df (df_with_null))
      answer = messagebox.askyesno('Rows with null data:','Do you want delete rows with null?')
      if answer == True:
         df_monthly.drop(df_with_null.index, inplace=True)
   buttSaveCSV.config(state="normal")
   buttInsertDB.config(state="normal")
   buttViewData.config(state="normal")
#############################################################################
def bSaveCSV():
   global df_monthly
   newfilename = get_nonexistant_path(file_path)
   df_monthly.to_csv(newfilename, header=False, index=False, encoding='utf-8')
   print ('File saved in ', newfilename)  
#############################################################################
def bInsertDB():
   global df_monthly
   insert_monthlytable (df_monthly)
#############################################################################
def bViewData():
   show(df_monthly, settings={'block': True})
#############################################################################
###########################################################################
def get_nonexistant_path(fname_path):
   
    if not os.path.exists(fname_path):
        return fname_path
    filename, file_extension = os.path.splitext(fname_path)
    i = 1
    new_fname = "{}-{}{}".format(filename, i, file_extension)
    while os.path.exists(new_fname):
        i += 1
        new_fname = "{}-{}{}".format(filename, i, file_extension)
    return new_fname
#########################################################################################
def insert_monthlytable (df):
   pymysql.converters.encoders[np.float64] = pymysql.converters.escape_float  #because error in
   pymysql.converters.conversions = pymysql.converters.encoders.copy()        # 'numpy.float64' object has no attribute 'translate'
   pymysql.converters.conversions.update(pymysql.converters.decoders)
   print("Start database conn")
   # Import dataframe into MySQL Enter your DB data
   user = '******'
   passw = '********'
   database = '*************'
   host =  '**********'
   port = ******
   try:
      conn = pymysql.connect(host=host,
                           port=port,
                           user=user, 
                           passwd=passw,  
                           db=database,
                           charset='utf8')
      if conn.open:
         db_Info = conn.get_server_info()
         print("Connected to MySQL Server version ", db_Info)
      cursor = conn.cursor()
      conn.autocommit = False
      skip_insert = False
      try:
         for ind in df.index:
            try:
               #print ('Format time record number ', ind )
               formatdate = df['DateTime'][ind].strftime('%Y-%m-%d %H:%M:%S')
            except ValueError as err:
               print("Failed to convert to DateTime: {}".format(err))
               answer = messagebox.askyesno('DateTime coversion error','Do you want skip one record?\nNo mean exit from inserting all data')
               if answer == True:
                  skip_insert = True
               else: 
                  cursor.close()
                  conn.close()
                  print("MySQL connection is closed")
                  return
            if not skip_insert: 
               sql = "INSERT INTO monthly_table (DateTime,Temperature,Humidity,DewPoint,WindSpeed,RecentHighGust,AverageWindBearing,RainfallRate,RainfallSoFar,SeaLevelPressure,RainfallCounter,InsideTemperature,InsideHumidity,CurrentGust,WindChill,HeatIndex,UVIndex,SolarRadiation,Evapotranspiration,AnnualEvapotranspiration,ApparentTemperature,MaxSolarRadiation,HoursOfSunshine,WindBearing,`RG-11Rain`,RainSinceMidnight) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"
               cursor.execute(sql, (formatdate,df['Temperature'][ind], df['Humidity'][ind], df['DewPoint'][ind], df['WindSpeed'][ind], df['RecentHighGust'][ind], df['AverageWindBearing'][ind], df['RainfallRate'][ind], df['RainfallSoFar'][ind], df['SeaLevelPressure'][ind], df['RainfallCounter'][ind], df['InsideTemperature'][ind], df['InsideHumidity'][ind], df['CurrentGust'][ind], df['WindChill'][ind], df['HeatIndex'][ind], df['UVIndex'][ind], df['SolarRadiation'][ind], df['Evapotranspiration'][ind], df['AnnualEvapotranspiration'][ind], df['ApparentTemperature'][ind], df['MaxSolarRadiation'][ind], df['HoursOfSunshine'][ind], df['WindBearing'][ind], df['RG-11Rain'][ind], df['RainSinceMidnight'][ind]))
               print ('Record number ', ind ,'inserted')
         conn.commit()
         print( ind," records saved to monthly_table")
      except conn.Error as error :
         print("Failed to update record to database rollback: {}".format(error))
         conn.rollback()
   except Exception as e:
      print("Error while connecting to MySQL", e)
   finally:
      if (conn.open):
         cursor.close()
         conn.close()
         print("MySQL connection is closed")

"""
   try:
      sql = "SELECT * FROM `monthly_table`"
      cursor.execute(sql)
      # Fetch all the records and use a for loop to print them one line at a time
      result = cursor.fetchall()

      for i in result:
         print(i)
   except Exception as ex:   
      print(ex)
   finally:
      if(conn.open):
         cursor.close()
         conn.close()
"""
########################################################################################
def to_print_df (df_in):
   df_in.to_csv("C:/Cumulus/data/Python/tmpfile.csv", header=False, index=False, encoding='utf-8')
   try:
      with open("C:/Cumulus/data/Python/tmpfile.csv") as f:
         printstring = f.read()
   except IOError as e:
      print("Couldn't open or write to file (%s)." % e)
   return (printstring)
########################################################################################

# A function for find spikes
def find_spikes(df_in, col_name, maxdata, mindata):
   df_out = df_in.loc[(df_in[col_name] > maxdata) | (df_in[col_name] < mindata)]
   if len(df_out.index.values) != 0:
      minidx = df_out.index.min()-1
      if df_in.index.min() > minidx:
         minidx = df_in.index.min()
      maxidx = df_out.index.max()+2
      if df_in.index.max() < maxidx:
         maxidx = df_in.index.max()
      coln = df_in.columns.get_loc(col_name)
      df_pr =  df_in.iloc[minidx:maxidx , [0 ,coln ]]
      list_of_df = [d for _, d in df_pr.groupby(df_pr.index - np.arange(len(df_pr)))]
      Ttm.delete(1.0,END)
      onedf = pd.concat(list_of_df)
      Ttm.insert(END,  to_print_df (onedf))

      global chart_type
      if (maxidx - minidx) > 5 :
         try: 
            chart_type.get_tk_widget().pack_forget()
         except: 
            pass               
         figure = plt.Figure(figsize=(6,5), dpi=100)
         ax = figure.add_subplot(111)
         df_pr.plot(kind='scatter',x='DateTime',y=col_name, color='blue',ax = ax)
         ax.set_title('Outlier chart')
         chart_type = FigureCanvasTkAgg(figure, down_frame)
         chart_type.get_tk_widget().pack()

      naslov = 'Rows with ' + col_name + ' spikes:'
      answer = messagebox.askyesno(naslov,'Do you want interpolate data?')
      if answer == True:
         ix_keep = df_out.index
         df_in.loc[ix_keep,[col_name]] = np.nan

         if pd.isna(df_in[col_name][0]):
            #df_in[col_name][0] = df_in[col_name][1]
            colu = df_in.columns.get_loc(col_name)
            df_in.iloc[0,colu] = df_in.iloc[1,colu]
            if pd.isna(df_in[col_name][0]):
               df_in.loc[ : , col_name] = mindata
               return df_in
         df_in[col_name] = df_in[col_name].interpolate(method ='linear', limit_direction ='forward')
         if col_name=='Evapotranspiration':
            df_in.loc[ix_keep,[col_name]] = df_in.loc[ix_keep,[col_name]].round(decimals=2)
         else:
            df_in.loc[ix_keep,[col_name]] = df_in.loc[ix_keep,[col_name]].round(decimals=1)
   return df_in
########################################################################################
# A function for find outliers
def find_outlier(df_in, col_name,iqr_set = 1.5):
   q1 = df_in[col_name].quantile(0.25)
   q3 = df_in[col_name].quantile(0.75)
   iqr = q3-q1 #Interquartile range
   fence_low  = q1-iqr_set*iqr
   fence_high = q3+iqr_set*iqr
   df_out = df_in.loc[(df_in[col_name] < fence_low) | (df_in[col_name] > fence_high)]
   if len(df_out.index.values) != 0:
      minidx = df_out.index.min()-1
      if df_in.index.min() > minidx:
         minidx = df_in.index.min()
      maxidx = df_out.index.max()+2
      if df_in.index.max() < maxidx:
         maxidx = df_in.index.max()
      coln = df_in.columns.get_loc(col_name)
      df_pr =  df_in.iloc[minidx:maxidx , [0 ,coln ]]
      list_of_df = [d for _, d in df_pr.groupby(df_pr.index - np.arange(len(df_pr)))]
      Ttm.delete(1.0,END)
      onedf = pd.concat(list_of_df)
      Ttm.insert(END,  to_print_df (onedf))
      global chart_type
      if (maxidx - minidx) > 5 :
         try: 
            chart_type.get_tk_widget().pack_forget()
         except: 
            pass               
         figure = plt.Figure(figsize=(6,5), dpi=100)
         ax = figure.add_subplot(111)
         df_pr.plot(kind='scatter',x='DateTime',y=col_name, color='blue',ax = ax)
         ax.set_title('Outlier chart')
         chart_type = FigureCanvasTkAgg(figure, down_frame)
         chart_type.get_tk_widget().pack()
      

      naslov = 'Rows with ' + col_name + ' outlier:'
      answer = messagebox.askyesno(naslov,'Do you want interpolate data?')
      if answer == True:
         ix_keep = df_out.index
         df_in.loc[ix_keep,[col_name]] = np.nan

         if pd.isna(df_in[col_name][0]):
            #df_in[col_name][0] = df_in[col_name][1]
            colu = df_in.columns.get_loc(col_name)
            df_in.iloc[0,colu] = df_in.iloc[1,colu]
         df_in[col_name] = df_in[col_name].interpolate(method ='linear', limit_direction ='forward')
         if col_name=='Evapotranspiration':
            df_in.loc[ix_keep,[col_name]] = df_in.loc[ix_keep,[col_name]].round(decimals=2)
         else:
            df_in.loc[ix_keep,[col_name]] = df_in.loc[ix_keep,[col_name]].round(decimals=1)
   return df_in

##########################################################################
def obrada_monthlytable(mystring):
   mystring = mystring.replace(',', '.')
   mystring = mystring.replace("\n\n","\n")
   #print(repr(mystring))
   print(mystring)
   lines = mystring.splitlines()
   #del lines[:1]
   #lines = lines[:-1]
   #print(lines)
   
   reader = csv.reader(lines, delimiter=';')
   
   df = pd.DataFrame.from_records(reader, columns=['Date','Time','Temperature','Humidity','DewPoint','WindSpeed','RecentHighGust','AverageWindBearing','RainfallRate','RainfallSoFar','SeaLevelPressure','RainfallCounter','InsideTemperature','InsideHumidity','CurrentGust','WindChill','HeatIndex','UVIndex','SolarRadiation','Evapotranspiration','AnnualEvapotranspiration','ApparentTemperature','MaxSolarRadiation','HoursOfSunshine','WindBearing','RG-11Rain','RainSinceMidnight'])
   total_rows = len(df)
   datetime_rec = list(range(total_rows))
   for ind in df.index:
      datetime_str = df['Date'][ind] + df['Time'][ind]
      datetime_rec[ind] = datetime.strptime(datetime_str, '%d.%m.%y%H:%M')
      #print(df['Time'][ind], df['InsideTemperature'][ind]) 
   df.drop(['Date', 'Time'], axis=1, inplace=True)
   all_columns = list(df)
   dfnumber = pd.DataFrame(all_columns)
   dfnumber = df.astype(float)
   #coln=['Temperature','Humidity','DewPoint','WindSpeed','RecentHighGust','AverageWindBearing','RainfallRate','RainfallSoFar','SeaLevelPressure','RainfallCounter','InsideTemperature','InsideHumidity','CurrentGust','WindChill','HeatIndex','UVIndex','SolarRadiation','Evapotranspiration','AnnualEvapotranspiration','ApparentTemperature','MaxSolarRadiation','HoursOfSunshine','WindBearing','RG-11Rain','RainSinceMidnight']
   #dfnumber = df[coln].apply(pd.to_numeric)
   #dfnumber.apply(pd.to_numeric)
   df['DateTime'] =  datetime_rec
   dfnumber['DateTime'] =  datetime_rec
   dfnumber = dfnumber.reindex(columns=['DateTime','Temperature','Humidity','DewPoint','WindSpeed','RecentHighGust','AverageWindBearing','RainfallRate','RainfallSoFar','SeaLevelPressure','RainfallCounter','InsideTemperature','InsideHumidity','CurrentGust','WindChill','HeatIndex','UVIndex','SolarRadiation','Evapotranspiration','AnnualEvapotranspiration','ApparentTemperature','MaxSolarRadiation','HoursOfSunshine','WindBearing','RG-11Rain','RainSinceMidnight'])
   #print (df)
   collist=['Temperature','Humidity','DewPoint','SeaLevelPressure','InsideTemperature','InsideHumidity','WindChill','HeatIndex','ApparentTemperature']
   for column in dfnumber.columns:
      if column in collist:
         df_number = find_outlier(dfnumber,column)
   df_number = find_spikes(df_number, 'WindSpeed', 60.0, 0.0)
   df_number = find_spikes(df_number, 'RecentHighGust', 140.0, 0.0)
   df_number = find_spikes(df_number, 'RainfallRate', 100.0, 0.0)
   df_number = find_spikes(df_number, 'RainfallSoFar', 200.0, 0.0)
   df_number = find_spikes(df_number, 'RainfallCounter', 1000.0, 0.0)
   df_number = find_spikes(df_number, 'CurrentGust', 140.0, 0.0)
   df_number = find_spikes(df_number, 'UVIndex', 14.0, 0.0)
   df_number = find_spikes(df_number, 'SolarRadiation', 1100.0, 0.0)
   df_number = find_spikes(df_number, 'Evapotranspiration', 1.0, -0.5)
   df_number = find_spikes(df_number, 'MaxSolarRadiation', 1000.0, 0.0)
   df_number = find_spikes(df_number, 'HoursOfSunshine', 15.0, 0.0)
   df_number = find_spikes(df_number, 'WindBearing', 330.0, 0.0)
   df_number = find_spikes(df_number, 'RainSinceMidnight', 200.0, 0.0)
   return(df_number)
###########################################################################
# Creating tkinter window 
window = tk.Tk() 
window.title('MeteologgerPy') 
window.geometry('1250x940') 
# Create 3 frames
up_frame = Frame(window, bg='grey',borderwidth=5, relief=RAISED)
up_frame.grid(row=0, column=0, padx=10, pady=5)
midle_frame = Frame(window, bg='grey',borderwidth=5, relief=RAISED)
midle_frame.grid(row=1, column=0, padx=10, pady=5) 
down_frame = Frame(window, bg='grey')
down_frame.grid(row=2, column=0, padx=10, pady=5)  
# everything for up_frame
ttk.Label(up_frame, text = "Files on Meteologger",  
          background='grey', foreground ="white",  
          font = ("Calibri", 15)).grid(row = 0, column = 1) 
   
ttk.Label(up_frame, text = " Select the File :",background='grey',
          font = ("Calibri", 10)).grid(column = 0, 
          row = 1, padx = 12, pady = 25)
ttk.Label(up_frame, text = "File data:", background='grey',
          font = ("Calibri", 12)).grid(column = 3, 
          row = 0, padx = 10, pady = 25)
# Combobox creation 
n = tk.StringVar() 
filechoosen = ttk.Combobox(up_frame, width = 27, textvariable = n) 
filechoosen['values'] = 'No connection' 
filechoosen.grid(column = 1, row = 1) 
try:
   sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   sock.connect((TCP_IP, TCP_PORT))
except OSError as msg:
   sock.close()
   sock = None
if sock is None:
    print('could not open socket')
    tk.messagebox.showwarning('Conn status', 'Not connection with Meteologger')
else:
   tk.messagebox.showinfo('Conn status', 'Establish connection\nwith Meteologger')
   sock.send( LISTM )
   data = sock.recv(1024)
   data = sock.recv(1024)
   lst = str(data, encoding='utf-8').splitlines()
   combolist = []

   for line in lst:
      line = line.split('/', 1)[1]
      line = line.split(' ', 1)[0]
      if '.txt' in line:
         combolist.append(line)
   print (combolist)
   sock.close()
   filechoosen['values'] = (combolist)

filechoosen.current() 
# Text box
Tt = Text(up_frame , height=4, width=106,  padx = 20, pady = 10,borderwidth=5, relief=SUNKEN,font = ("Calibri", 10))
Tt.grid(row=1, column=3)
# Button
buttLoadML=Button(up_frame, text=" Load file ",command=bLoadML)
buttLoadML.grid(row=1,column=2,padx = 5, pady = 10)
buttSaveML=Button(up_frame, text=" Save file ",state=DISABLED,command=bSaveML)
buttSaveML.grid(row=1,column=4,padx = 5, pady = 10)
##############################################################################
# everything for midle_frame
ttk.Label(midle_frame, text = "Data processing",  
          background='grey', foreground ="white",  
          font = ("Calibri", 15)).grid(row = 0, column = 1) 
   
ttk.Label(midle_frame, text = "Data:", background='grey',
          font = ("Calibri", 12)).grid(column = 3, 
          row = 0, padx = 10, pady = 25)
# Button
buttLoadFL=Button(midle_frame, text=" Load file ",command=bLoadFL)
buttLoadFL.grid(row=1,column=0,padx = 10, pady = 10)
buttProcDT=Button(midle_frame, text=" Process data ",state=DISABLED,command=bProcDT)
buttProcDT.grid(row=1,column=1)
buttSaveCSV=Button(midle_frame, text=" Save to CSV ",state=DISABLED,command=bSaveCSV)
buttSaveCSV.grid(row=0,column=4,padx = 10, pady = 10)
buttViewData=Button(midle_frame, text="  View Data  ",state=DISABLED,command=bViewData)
buttViewData.grid(row=1,column=4,padx = 10, pady = 10)
buttInsertDB=Button(midle_frame, text=" Insert to DB ",state=DISABLED,command=bInsertDB)
buttInsertDB.grid(row=2,column=4,padx = 10, pady = 10)
# Text box
Ttm = Text(midle_frame, height=5, width=125,  padx = 20, pady = 10,borderwidth=5, relief=SUNKEN,font = ("Calibri", 10))
Ttm.grid(row=1, column=3)

window.mainloop() 