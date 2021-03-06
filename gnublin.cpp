//********************************************
//GNUBLIN API -- MAIN FILE
//build date: 06/10/13 11:30
//******************************************** 

#include"gnublin.h"


//Converting string to number
int stringToNumber(std::string str){
	std::stringstream  strin;
	int var;

	strin << str;
	strin >> var;
	
	return var;
}

//Converting a number to string
std::string numberToString(int num){
	std::ostringstream strout;
	std::string str;

	strout << num;
	str = strout.str();

	return str;
}

//Converting string wich repesenting a hexnumber to number
int hexstringToNumber(std::string str){
	std::stringstream  strin;
	int var;

	strin << std::hex << str;
	strin >> var;
	
	return var;
}

/** @~english 
* @brief Reset the ErrorFlag.
*
* @~german 
* @brief Setzt das ErrorFlag zurück.
*
*/
gnublin_gpio::gnublin_gpio(){
	error_flag = false;
}



/** @~english 
* @brief Removes the GPIO. 
*
* Removes the GPIO from the filesystem, after that, you cannot access the pin.
* @return bool error_flag
*
* @~german 
* @brief Entferne GPIO
*
* Entfernt den GPIO aus dem Filesystem, es kann kein Zugriff mehr auf diesen erfolgen.
* @return bool error_flag
*/
int gnublin_gpio::unexport(int pin){
	std::string pin_str = numberToString(pin);
	std::string dir = "/sys/class/gpio/unexport";
	std::ofstream file (dir.c_str());
	if (file < 0) {
		error_flag = true;
		return -1;
	}
	file << pin_str;
	file.close();
	error_flag = false;
	return 1;
}


/** @~english 
* @brief Returns the error flag. 
*
* If something went wrong, the flag is true.
* @return bool error_flag
*
* @~german 
* @brief Gibt das Error Flag zurück.
*
* Falls das Error Flag in der Klasse gesetzt wurde, wird true zurück gegeben, anderenfalls false.
* @return bool error_flag
*/
bool gnublin_gpio::fail(){
	return error_flag;
}

/** @~english 
* @brief Get the last Error Message.
*
* This Funktion returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_gpio::getErrorMessage(){
	return ErrorMessage.c_str();
}

/** @~english 
* @brief Change the PinMode.
*
* With this function you can set the pin as input or output. <br>
* This must be done before all other operations. <br><br>
* Hint: GPIO 4 can only be used as output.
* @param pin Pin number to change the mode
* @param direction direction of the pin (OUTPUT, INPUT)
* @return success: 1, failure: -1
*
* @~german 
* @brief Pin Modi festlegen.
*
* Mit dieser Funktion kann der Modi des Pins festgelegt werden, also ob er als Eingang oder Ausgang genutzt werden soll. <br>
* Vor allen anderen Zugriffen muss diese Funktion ausgeführt werden. <br> <br>
* Hinweis: GPIO 4 kann auf dem GNUBLIN board nur als Ausgang genutzt werden. 
* @param pin Nummer des GPIO-Pins
* @param direction Richtung des Pins (OUTPUT, INPUT) 
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_gpio::pinMode(int pin, std::string direction){
	#if (BOARD != RASPBERRY_PI)
	if (pin == 4 && direction == "out"){
		error_flag = true;
		return -1;
	}
	#endif
	std::string pin_str = numberToString(pin);
	std::string dir = "/sys/class/gpio/export";
	std::ofstream file (dir.c_str());
	if (file < 0) {
		error_flag = true;
		return -1;
	}
	file << pin;
	file.close();

	dir = "/sys/class/gpio/gpio" + pin_str + "/direction";

	file.open(dir.c_str());
	if (file < 0) {
		error_flag = true;
		return -1;
	}
	file << direction;
	file.close();
	error_flag = false;
	return 1;
}


/** @~english 
* @brief Write Pin.
*
* Set the Pin to the given value. 
* @param pin Pin number to change the mode
* @param value Value of the Pin (LOW, HIGH)
* @return success: 1, failure: -1
*
* @~german 
* @brief Pin schreiben.
*
* Den GPIO-Pin auf einen bestimmten Wert setzen. 
* @param pin Nummer des entsprechenden GPIO-Pins
* @param value Wert auf den der Pin gesetzt werden soll (LOW, HIGH) 
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_gpio::digitalWrite(int pin, int value){
	#if (BOARD != RASPBERRY_PI)
	if (pin == 4){
		error_flag = true;
		return -1;
	}
	#endif
	if (value != 0 && value != 1){
		error_flag = true;
		return -1;
	}
	std::string value_str = numberToString(value);
	std::string pin_str = numberToString(pin);
	std::string dir = "/sys/class/gpio/gpio" + pin_str + "/value";

	std::ofstream file (dir.c_str());
	if (file < 0) {
		error_flag = true;
		return -1;
	}
	file << value_str;
	file.close();
	error_flag = false;
	return 1;
}

/** @~english 
* @brief Read Pin.
*
* Reads the value of the given pin. 
* @param pin Pin number to read
* @return Value of the GPIO-Pin (0,1), -1 in case of failure 
*
* @~german 
* @brief Pin lesen.
*
* Abfragen des GPIO-Wertes. 
* @param pin Nummer des entsprechenden GPIO-Pins
* @return Wert des GPIO-Pins (0,1), -1 im Fehlerfall 
*/
int gnublin_gpio::digitalRead(int pin) {
	std::string value;
	
	std::string pin_str = numberToString(pin);
	std::string device = "/sys/class/gpio/gpio" + pin_str + "/value";
	std::ifstream file(device.c_str());
	if (file < 0){
		error_flag = true;
		return -1;
	}
	file >> value;
	file.close();
	error_flag = false;
	return stringToNumber(value);
	
}


//*******************************************************************
//Class for accessing GNUBLIN i2c Bus
//*******************************************************************

//------------------Konstruktor------------------
/** @~english 
* @brief Sets the error_flag to "false" and the devicefile to "/dev/i2c-1"
*
* @~german 
* @brief Setzt das error_flag auf "false" und das devicefile auf standardmäßig "/dev/i2c-1"
*
*/
gnublin_i2c::gnublin_i2c()
{
	devicefile="/dev/i2c-1";
	error_flag=false;
}

//-------------------------------Fail-------------------------------
/** @~english 
* @brief returns the error flag to check if the last operation went wrong
*
* @return error_flag as boolean
*
* @~german 
* @brief Gibt das error_flag zurück um zu überprüfen ob die vorangegangene Operation einen Fehler auweist
*
* @return error_flag als bool
*/
bool gnublin_i2c::fail(){
	return error_flag;
}

//-------------set Address-------------
/** @~english 
* @brief Set the i2c slave address 
*
* With this function you can set the individual I2C Slave-Address.
* @param Address new I2C slave Address
*
* @~german 
* @brief Setzt die i2c slave Adresse
*
* Mit dieser Funktion kann die individuelle I2C Slave-Adresse gesetzt werden.
* @param Address neue I2C slave Adresse
*/
void gnublin_i2c::setAddress(int Address){
	slave_address = Address;
}

//-------------get Address-------------
/** @~english 
* @brief Get the i2c slave address 
*
* With this function you can get the set Slave-Address.
* @return Address I2C slave Address
*
* @~german 
* @brief gibt die gesetzte I2C Slave Adresse zurück
*
* Mit dieser Funktion kann die gesetzte I2C Slave-Adresse ausgelesen werden.
* @return Address I2C slave Address
*/
int gnublin_i2c::getAddress(){
	return slave_address;
}

//-------------get Error Message-------------
/** @~english 
* @brief Get the last Error Message.
*
* This function returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_i2c::getErrorMessage(){
	return ErrorMessage.c_str();
}

//-------------------set devicefile----------------
/** @~english
* @brief set i2c the device file. default is "/dev/i2c-1"
*
* This function sets the devicefile you want to access. by default "/dev/i2c-1" is set.
* @param filename path to the devicefile e.g. "/dev/i2c-0"
*
* @~german
* @brief setzt die I2C Device Datei. Standard ist die "/dev/i2c-1"
*
* Diese Funktion setzt die Geräte Datei, auf die man zugreifen möchte. Standardmäßig ist bereits "/dev/i2c-1" gesetzt.
* @param filename Dateipfad zur Geräte Datei, z.B. "/dev/i2c-0"
*/
void gnublin_i2c::setDevicefile(std::string filename){
	devicefile = filename;
}


//----------------------------------receive----------------------------------
/** @~english 
* @brief receive bytes from the I2C bus.
*
* This function reads "length" number of bytes from the i2c bus and stores them into the "RxBuf". At success the function returns 1, on failure -1.<br>
* e.g.<br>
* read 2 bytes into buf<br>
* receive(buf, 2);
* @param RxBuf Receive buffer. The read bytes will be stored in it.
* @param length Amount of bytes that will be read.
* @return success: 1, failure: -1
*
* @~german 
* @brief Empfängt Bytes vom I2C Bus.
*
* Diese Funktion liest "length" Anzahl an Bytes vom I2C Bus und speichert diese in "RxBuf". Bei Erfolg wird 1 zurück gegeben, bei Misserfolg -1.<br>
* Beispiele:<br>
* Lese 2 Bytes und speichere diese in "buf":<br>
* receive(buf, 2);
* @param RxBuf Empfangs Puffer. Die gelesenen Bytes werden hier gespeichert.
* @param length Anzahl der zu lesenden Bytes.
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_i2c::receive(unsigned char *RxBuf, int length){
	error_flag=false;
	int fd;


	if ((fd = open(devicefile.c_str(), O_RDWR)) < 0) {
		ErrorMessage="ERROR opening: " + devicefile + "\n";
		error_flag=true;
		close(fd);    	
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, slave_address) < 0) {
		ErrorMessage="ERROR address: " + numberToString(slave_address) + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
  	}

	if (read(fd, RxBuf, length) != length){
		ErrorMessage="i2c read error! Address: " + numberToString(slave_address) + " dev file: " + devicefile + "\n";		
		error_flag=true; 
		close(fd);
		return -1;
	}

	close(fd); 
	return 1;
}

//----------------------------------receive----------------------------------
/** @~english 
* @brief receive bytes from the I2C bus.
*
* This function reads "length" number of bytes from the register "RegisterAddress" and stores them into the "RxBuf". At success the function returns 1, on failure -1.<br>
* e.g.<br>
* read 2 bytes into buf<br>
* receive(buf, 2);<br><br>
* 
* read 3 bytes into buf from a register with the address 0x12<br>
* receive(0x12, buf, 3);
* @param RegisterAddress Address of the register you want to read from
* @param RxBuf Receive buffer. The read bytes will be stored in it.
* @param length Amount of bytes that will be read.
* @return success: 1, failure: -1
*
* @~german 
* @brief Empfängt Bytes vom I2C Bus.
*
* Diese Funktion liest "length" Anzahl an Bytes aus dem Register "RegisterAddress" und speichert diese in "RxBuf". Bei Erfolg wird 1 zurück gegeben, bei Misserfolg -1.<br>
* Beispiele:<br>
* Lese 2 Bytes und speichere diese in "buf":<br>
* receive(buf, 2);<br><br>
*
* Lese 3 Bytes aus Register 0x12 und speichere sie in "buf":<br>
* receive(0x12, buf, 3);
* @param RegisterAddress Adresse des zu lesenden Registers.
* @param RxBuf Empfangs Puffer. Die gelesenen Bytes werden hier gespeichert.
* @param length Anzahl der zu lesenden Bytes.
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_i2c::receive(unsigned char RegisterAddress, unsigned char *RxBuf, int length){
	error_flag=false;	
	int fd;

	if ((fd = open(devicefile.c_str(), O_RDWR)) < 0) {
		ErrorMessage="ERROR opening: " + devicefile + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
	}

	if (ioctl(fd, I2C_SLAVE, slave_address) < 0) {
		ErrorMessage="ERROR address: " + numberToString(slave_address) + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
  	}

	if (write(fd, &RegisterAddress, 1) != 1){
		ErrorMessage="i2c write error!\n";
		error_flag=true; 
		close(fd);
		return -1;
		}


	if (read(fd, RxBuf, length) != length){
		ErrorMessage="i2c read error! Address: " + numberToString(slave_address) + " dev file: " + devicefile + "\n";
		error_flag=true; 
		close(fd);
		return -1;
	}

	close(fd); 	 
	return 1;
}

//----------------------------------send----------------------------------
/** @~english 
* @brief send bytes to the I2C bus.
*
* This function sends "length" number of bytes from the "TxBuf" to the i2c bus. At success the function returns 1, on failure -1.<br>
* e.g.<br>
* send 2 bytes from buf to the I2C bus<br>
* send (buf, 2);
* @param TxBuf Transmit buffer. The bytes you want to send are stored in it.
* @param length Amount of bytes that will be send.
* @return success: 1, failure: -1
*
* @~german 
* @brief sendet Bytes an den I2C Bus.
*
* Diese Funktion sendet "length" Anzahl an Bytes aus dem "TxBuf" an den I2C Bus. Bei Erfolg wird 1 zurück gegeben, bei Misserfolg -1.<br>
* Beispiele:<br>
* Sende 2 Bytes von "buf" an den i2c Bus:
* send(buf, 2);
* @param RxBuf Sende Puffer. Die zu sendenden Bytes sind hier gespeichert.
* @param length Anzahl der zu sendenden Bytes.
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_i2c::send(unsigned char *TxBuf, int length){
	error_flag=false;	
	int fd; 

	if ((fd = open(devicefile.c_str(), O_RDWR)) < 0) {
		ErrorMessage="ERROR opening: " + devicefile + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
	}

	if (ioctl(fd, I2C_SLAVE, slave_address) < 0) {
		ErrorMessage="ERROR address: " + numberToString(slave_address) + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
  	}

	if(write(fd, TxBuf, length) != length){
		ErrorMessage="i2c write error!\n";
		error_flag=true; 
		close(fd);
		return -1;
	}

	close(fd);	
	return 1;
}

//----------------------------------send----------------------------------
/** @~english 
* @brief send bytes to the I2C bus.
*
* This function sends "length" number of bytes from the "TxBuf" to the register "RegisterAddress". At success the function returns 1, on failure -1.<br>
* e.g.<br>
* send 2 bytes from buf to the I2C bus<br>
* send (buf, 2);<br><br>
*
* send 3 bytes from buf to a register with the address 0x12<br>
* send (0x12, buf, 3);
* @param RegisterAddress Address of the register you want to send the bytes to
* @param TxBuf Transmit buffer. The bytes you want to send are stored in it.
* @param length Amount of bytes that will be send.
* @return success: 1, failure: -1
*
* @~german 
* @brief sendet Bytes an den I2C Bus.
*
* Diese Funktion sendet "length" Anzahl an Bytes aus dem "TxBuf" an das Register "RegisterAddress". Bei Erfolg wird 1 zurück gegeben, bei Misserfolg -1.<br>
* Beispiele:<br>
* Sende 2 Bytes von "buf" an den i2c Bus:
* send(buf, 2);<br><br>
*
* Sende 3 Bytes aus "buf" an das Register mit der Adresse 0x12:<br>
* send(0x12, buf, 3);
* @param RegisterAddress Adresse des Registers in das man schreiben will.
* @param RxBuf Sende Puffer. Die zu sendenden Bytes sind hier gespeichert.
* @param length Anzahl der zu sendenden Bytes.
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_i2c::send(unsigned char RegisterAddress, unsigned char *TxBuf, int length){
	error_flag=false;	
	int fd, i;
	unsigned char data[length+1];
	data[0]=RegisterAddress;

	for ( i = 0; i < length ; i++ ) {
		data[ i + 1 ] = (char)TxBuf[ i ];
	}

	if ((fd = open(devicefile.c_str(), O_RDWR)) < 0) {
		ErrorMessage="ERROR opening: " + devicefile + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
	}

	if (ioctl(fd, I2C_SLAVE, slave_address) < 0) {
		ErrorMessage="ERROR address: " + numberToString(slave_address) + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
  	}
	
	
	if(write(fd, data, length+1) != length+1){
		ErrorMessage="i2c write error!\n";
		error_flag=true; 
		close(fd);
		return -1;
	}

	close(fd);	
	return 1;
}

//----------------------------------send----------------------------------
/** @~english 
* @brief send a byte to the I2C bus.
*
* This function sends a byte to the i2c bus. At success the function returns 1, on failure -1.<br>
* e.g.<br>
* send 0xff to the I2C bus<br>
* send (0xff);
* @param value byte that will be send.
* @return success: 1, failure: -1
*
* @~german 
* @brief sendet 1 Byte an den I2C Bus.
*
* Diese Funktion sendet ein byte an den i2c Bus. Bei Erfolg wird 1 zurück gegeben, bei Misserfolg -1.<br>
* Beispiel:<br>
* Sende 0xff an den i2c Bus:<br>
* send(0xff);
* @param value Byte das gesendet wird.
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_i2c::send(int value){
	error_flag=false;
	int buffer[1];
	buffer[0]=value;	
	int fd; 

	if ((fd = open(devicefile.c_str(), O_RDWR)) < 0) {
		ErrorMessage="ERROR opening: " + devicefile + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
	}

	if (ioctl(fd, I2C_SLAVE, slave_address) < 0) {
		ErrorMessage="ERROR address: " + numberToString(slave_address) + "\n";
		error_flag=true; 
		close(fd);
    	return -1;
  	}

	if(write(fd, buffer, 1) != 1){
		ErrorMessage="i2c write error!\n";
		error_flag=true; 
		close(fd);
		return -1;
	}

	close(fd);	
	return 1;
}


//***************************************************************************
// Class for accessing the SPI-Bus
//***************************************************************************

/**
* @~english
* @brief Loads the standard SPI driver if necessary.
*
* Default chipselect:
* GNUBLIN: CS = 11
* RASPBERRY PI: CS = 0
* @~german
* @brief Die Standart SPI-Treiber werden geladen, wenn sie noch nicht vorhanden sind.
*
* Default chipselect:
* GNUBLIN: CS = 11
* RASPBERRY PI: CS = 0
*/
gnublin_spi::gnublin_spi(){
	error_flag = false;
	#if BOARD == RASPBERRY_PI
	std::string device = "/dev/spidev0.0";
	#else
	std::string device = "/dev/spidev0.11";
	#endif
	fd = open(device.c_str(), O_RDWR);
	if (fd < 0) {
		#if BOARD == RASPBERRY_PI
		system("modprobe spi-bcm2708");
		#else
		system("modprobe spidev cs_pin=11");
		#endif
		sleep(1);
		fd = open(device.c_str(), O_RDWR);
	}
}


//******************** destructor *******************************************

gnublin_spi::~gnublin_spi(){
	close(fd);
}


//******************** fail() ***********************************************
/**
* @~english
* @brief Returns the errorflag to detect error in the previous called method.
*
* @return error_flag
*
* @~german
* @brief Gibt das errorflag zurück, um Fehler in der zuvor aufgerugfenen Methode zu erkennen.
*
* @return error_flag
*/
bool gnublin_spi::fail(){
	return error_flag;
}


//-------------get Error Message-------------
/**
* @~english
* @brief Returns the ErrorMessage of the previous error if one exist.
*
* @return ErrorMessage as C-String
*
* @~german
* @brief Gibt die Fehlernachricht des zuvor aufgetretenen Fehlers zurück, wenn weine exisitert.
*
* @return ErrorMessage als C-String
*/
const char *gnublin_spi::getErrorMessage(){
	return ErrorMessage.c_str();
}


//*********************** setCS *********************************************

/**
* @~english
* @brief Set the custom chipselect pin
*
* @param cs Number of the chipselect-pin
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt den benutzerdefinierten Chipselect-Pin.
*
* @param cs Nummer des Chipselect-Pin
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::setCS(int cs){
	std::string cs_str = numberToString(cs);
	std::string device = "/dev/spidev0." + cs_str;
	fd = open(device.c_str(), O_RDWR);
	if (fd < 0) {
		#if (BOARD == RASPBERRY_PI)
		std::string command = "modprobe spi-bcm2708 cs_pin=" + cs_str;
		#else
		std::string command = "modprobe spidev cs_pin=" + cs_str;
		#endif
		system(command.c_str());
		sleep(1);
		fd = open(device.c_str(), O_RDWR);
		if (fd < 0){
			error_flag = true;
			return -1;
		}
	}
	error_flag = false;
	return 1;
}


//******************** set Mode *********************************************

/**
* @~english
* @brief Set the SPI-mode
*
* @param mode Number of the SPI-Mode
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt den SPI-Modus
*
* @param mode Nummer SPI-Modus
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::setMode(unsigned char mode){
	if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//***************** getMode *************************************************

/**
* @~english
* @brief Returns the set SPI-Mode
*
* @return Number of the SPI-mode
*
* @~german
* @brief Gibt den eingestellten SPI-Modus zurück
*
* @return Nummer des SPI-Modus
*/
int gnublin_spi::getMode(){
	__u8 mode;
	if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return mode;
}


//******************** setLSB ***********************************************

/**
* @~english
* @brief Set the LSB-mode
*
* @param lsb 0: MSB first, 1 LSB first
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt den LSB-Modus.
*
* @param lsb 0: MSB zuerst; 1 LSB zuerst
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::setLSB(unsigned char lsb){
	if (ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//************************ getLSB() *****************************************

/**
* @~english
* @brief Returns the set LSB-Mode
*
* @return 0: MSB first, 1 LSB first
*
* @~german
* @brief Gibt den eingestellten LSB-Modus zurück.
*
* @return 0: MSB zuerst; 1 LSB zuerst
*/
int gnublin_spi::getLSB(){
	__u8 lsb;
	if (ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb) < 0) {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return lsb;
}


//*********************** setLength() ***************************************

/**
* @~english
* @brief Set the lenght of the words which will be send
*
* @param bits Number of bits of each word
* @return 1 by success, -1 by failure
*
* @~german
* @brief Legt die Länge der gesendeten Wörter fest
*
* @param bits Anzahl der Bits je Word
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::setLength(unsigned char bits){
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//************************ getLength() **************************************

/**
* @~english
* @brief Returns the length of the words in bits
*
* @return Number of bits of each word
*
* @~german
* @brief Gibt die Anzahl von Bits je Wort zurück.
*
* @return anzahl der Bits je Wort
*/
int gnublin_spi::getLength(){
	__u8 bits;
	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return bits;
}


//************************* setSpeed ****************************************

/**
* @~english
* @brief Set the speed of the SPI-Bus
*
* @param speed Speed in Hz
* @return 1 by success, -1 by failure
*
* @~german
* @brief Legt die Geschwindigkeit des SPI-Buses fest.
*
* @param speed Geschwindigkeit in Hz
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::setSpeed(unsigned int speed){
	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = true;
	return 1;
}


//************************* getSpeed() **************************************

/**
* @~english
* @brief Returns the speed of the SPI-Bus
*
* @return SPeed in Hz
*
* @~german
* @brief Gibt die Geschwindigkeit des SPI-Buses zurück.
*
* @return Geschwindigkeit in Hz
*/
int gnublin_spi::getSpeed(){
	__u32 speed;
	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return speed;
}

//**************************** receive **************************************

/**
* @~english
* @brief Receive data from SPI Bus
*
* @param buffer Buffer for recived datas
* @param len Length of recived data
* @return 1 by success, -1 by failure
*
* @~german
* @brief Empfängt Daten über den SPI-Bus
*
* @param buffer Buffer für die empfangenen Daten
* @param len Anzahl der zu empfangenden Zeichen
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::receive(char* buffer, int len){
	if (read(fd, buffer, len) < 0) {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}

//*************************** send() ****************************************

/**
* @~english
* @brief Send data over the SPI Bus
*
* @param tx Datas which will be send
* @param length Length of datas
* @return 1 by success, -1 by failure
*
* @~german
* @brief Sendet Daten über den SPI-Bus
*
* @param tx Zu sendende Daten
* @param length Anzahl der zu sendenden Zeichen
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::send(unsigned char* tx, int length){
	int status;
	struct spi_ioc_transfer	xfer;
	xfer.tx_buf = (unsigned long)tx;
	xfer.len = length;
	xfer.rx_buf = 0;
	xfer.delay_usecs = 0;
	xfer.speed_hz = 0;
	xfer.bits_per_word = 0;
	status = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
	if ( status < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//****************************** message() ********************************

/**
* @~english
* @brief Send and recive data over the SPI-Bus (half duplex)
*
* @param tx Data which will be send
* @param tx_length Length of data which will be send
* @param rx Buffer for recived datas
* @param rx_length length of recived datas
* @return 1 by success, -1 by failure
*
* @~german
* @brief Sendet und empfängt daten über den SPI-Bus (halb duplex).
*
* @param tx Zu sendende Daten
* @param tx_length Anzahl der zu sendenden Zeichen
* @param rx Buffer für den Datenempfang
* @param rx_length Anzahl der zu empfangenden Zeichen
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_spi::message(unsigned char* tx, int tx_length, unsigned char* rx, int rx_length){
	int status;
	struct spi_ioc_transfer xfer[2];
	xfer[0].tx_buf = (unsigned long) tx;
	xfer[0].len = tx_length;
	xfer[0].rx_buf = 0;
	xfer[0].delay_usecs = 0;
	xfer[0].speed_hz = 0;
	xfer[0].bits_per_word = 0;
	
	xfer[1].rx_buf = (unsigned long) rx;
	xfer[1].len = rx_length;
	xfer[1].tx_buf = 0;
	xfer[1].delay_usecs = 0;
	xfer[1].speed_hz = 0;
	xfer[1].bits_per_word = 0;

	status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
	if (status < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


#if (BOARD != RASPBERRY_PI)
//****************************************************************************
// Class for easy acces to the GPAs
//****************************************************************************
/** @~english 
* @brief Opens the file stream and loads the kernel object lpc313x_adc.
*
* @~german 
* @brief Erzeugt den Datei stream und lädt das Kernelmodul lpc313x_adc.
*
*/
gnublin_adc::gnublin_adc(){
	std::ifstream file("/dev/lpc313x_adc");
	if (file.fail()) {
		system("modprobe lpc313x_adc");
		sleep(1);
	}
	file.close();
	error_flag = false;
}

//-------------fail-------------
/** @~english 
* @brief Returns the error flag. 
*
* If something went wrong, the flag is true.
* @return bool error_flag
*
* @~german 
* @brief Gibt das Error Flag zurück.
*
* Falls das Error Flag in der Klasse gesetzt wurde, wird true zurück gegeben, anderenfalls false.
* @return bool error_flag
*/
bool gnublin_adc::fail(){
	return error_flag;
}


//-------------getErrorMessage-------------
/** @~english 
* @brief Get the last Error Message.
*
* This Funktion returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_adc::getErrorMessage(){
	return ErrorMessage.c_str();
}


//-------------getValue-------------
/** @~english 
* @brief Get Value.
*
* This Funktion returns the Value from the register of the given pin.
* @param pin The pin, which should be used
* @return Value of the Pin - in failure: -1
*
* @~german 
* @brief Gibt den Wert des Pins zurück.
*
* Mit dieser Funktion erhält man den gemessenen Rohwert des angegebenen Pins, also den Wert des Registers. 
* @param pin Gibt den ADC-Pin an, von dem gemessen werden soll 
* @return Wert des ADCs, im Fehlerfall -1
*/
int gnublin_adc::getValue(int pin){
	std::string value;
	
	std::string pin_str = numberToString(pin);
	std::string device = "/dev/lpc313x_adc";
	std::ofstream file(device.c_str());
	if (file < 0) {
		error_flag = true;
		return -1;
	}
	file << pin_str;
	file.close();
	std::ifstream dev_file(device.c_str());
	dev_file >> value;
	dev_file.close();
	error_flag = false;
	return hexstringToNumber(value);
}

//-------------getVoltage-------------
/** @~english 
* @brief Get Voltage.
*
* This Funktion returns the Voltage of the given pin.
* @param pin The pin, which should be used
* @return Voltage of the Pin - in failure: -1
*
* @~german 
* @brief Ließt Spannung.
*
* Liefert den gemessenen Wert in mV. 
* @param pin Gibt den ADC-Pin an, von dem gemessen werden soll 
* @return Spannung des ADCs in mV, im Fehlerfall -1
*/
int gnublin_adc::getVoltage(int pin){
	int value = getValue(pin);
	value = value*825/256;
	return value;
}

//-------------setReference-------------
/** @~english 
* @brief set Reference.
*
* @param ref 
* @return success: 1, failure: -1
*
* @~german 
* @brief setzt Referenz.
*
* @param ref 
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_adc::setReference(int ref){
	error_flag = false;
	return 1;
}



#endif

//***************************************************************************
// Class for accesing the GNUBLIN MODULE-DISPLAY 2x16
//***************************************************************************


//********** Constructor ***************************

/**
* @~english
* @brief Set the default RS-PIN
*
* Default RS-Pin:
* GNUBLIN: GPIO14
* RASPBERRY PI: GPIO4
*
* @~german
* @brief Setzt die standard RS-Pins
*
* Standard RS-Pin:
* GNUBLIN: GPIO14
* RASPBERRY PI: GPIO4
*/
gnublin_module_dogm::gnublin_module_dogm(){

#if (BOARD == RASPBERRY_PI)
	rs_pin = 4;
#else
	rs_pin = 14;
#endif
	gpio.pinMode(rs_pin, OUTPUT);
	init_flag = false;
	error_flag = false;	
}

//********* init()**********************************
// execute initialisation of the display
// parameters: 	NONE
// return: 	* [int] 1	for success
//		* [int] -1  	for failure

/**
* @~english
* @brief Initialisizes the Display
*
* @return 1 by success, -1 by failure
*
* @~german
* @brief Initialisiert das Display
*
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::init(){
	__u8 init_str[] = {0x39, 0x14, 0x55, 0x6D, 0x78, 0x38, 0x0C, 0x01, 0x06};
	if (gpio.digitalWrite(rs_pin, LOW) <0){
			error_flag = true;
			return -1;
	}
	spi.setSpeed(100000);
	if (spi.send(init_str, 9) < 0){
		error_flag = true;
		return -1;
	}
	init_flag = true;
	error_flag = false;
	return 1;
}


//************ fail() *******************************

/**
* @~english
* @brief Returns the errorflag to detect error in the previous called method.
*
* @return error_flag
*
* @~german
* @brief Gibt das errorflag zurück, um Fehler in der zuvor aufgerugfenen Methode zu erkennen.
*
* @return error_flag
*/
bool gnublin_module_dogm::fail(){
	return error_flag;
}


//-------------get Error Message-------------
/**
* @~english
* @brief Returns the ErrorMessage of the previous error if one exist.
*
* @return ErrorMessage as C-String
*
* @~german
* @brief Gibt die Fehlernachricht des zuvor aufgetretenen Fehlers zurück, wenn weine exisitert.
*
* @return ErrorMessage als C-String
*/
const char *gnublin_module_dogm::getErrorMessage(){
	return ErrorMessage.c_str();
}


//*********** setRsPin() *****************************

/**
* @~english
* @brief Set the custom RS pin
*
* @param pin Number of the RS-pin
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt den benutzerdefinierten RS-Pin.
*
* @param pin Nummer des RS-Pin
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::setRsPin(int pin){
	rs_pin = pin;
	gpio.pinMode(rs_pin, OUTPUT);
	return 1;
}


//*********** setCS ************************************

/**
* @~english
* @brief Set the custom chipselect pin
*
* @param cs Number of the chipselect-pin
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt den benutzerdefinierten Chipselect-Pin.
*
* @param cs Nummer des Chipselect-Pin
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::setCS(int cs){
	if (spi.setCS(cs) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//************ print() **********************************

/**
* @~english
* @brief Print string on display
*
* @param buffer buffor of the string
* @return 1 by success, -1 by failure
*
* @~german
* @brief Schreibt String auf das Display
*
* @param buffer Buffer für den String
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::print(char* buffer){
	__u8 tmp[32];
	int len = strlen(buffer);
	if(!init_flag){
		init();
	}
	for(int i=0; i<len; i++){
		tmp[i] = buffer[i];
	}
	gpio.digitalWrite(rs_pin, HIGH);
	if (spi.send(tmp, len) < 0){
		gpio.digitalWrite(rs_pin, LOW);
		error_flag = true;
		return -1;
	}
	gpio.digitalWrite(rs_pin, LOW);
	error_flag = false;
	return 1;
}


//************ print() **********************************

/**
* @~english
* @brief Print string on a specific line of the display
*
* @param buffer buffor of the string
* @param line number of the line (1,2)
* @return 1 by success, -1 by failure
*
* @~german
* @brief Schreibt String in eine bestimmte Zeile des Displays
*
* @param buffer Buffer für den String
* @param line Zeilenenummer (1,2)
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::print(char* buffer, int line){
	error_flag = false;
	if(!init_flag){
		init();
	}
	if (returnHome() < 0){
		return -1;
	}
	if (line == 1){
		print(buffer);
	}
	else if (line == 2) {
		offset(16);
		print(buffer);
	}
	else {
		error_flag = true;
		return -1;
	}
	return -1;
}


//************ print() **********************************

/**
* @~english
* @brief Print string on a specific line with given offset on the display
*
* @param buffer buffor of the string
* @param line number of the line (1,2)
* @param off number of elements for the offset
* @return 1 by success, -1 by failure
*
* @~german
* @brief Schreibt String in eine bestimmte Zeile mit gegebenem Offset auf das Displays
*
* @param buffer Buffer für den String
* @param line Zeilenenummer (1,2)
* @param off Anzahl der Positionen, um die verschoben werden soll
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::print(char* buffer, int line, int off){
	error_flag = false;
	if(!init_flag){
		init();
	}
	returnHome();
	if (line == 1){
		offset(off);
		print(buffer);
	}
	else if (line == 2) {
		off +=16;
		offset(off);
		print(buffer);
	}
	else {
		error_flag = true;
		return -1;
	}
	return -1;
}


//****************** offset() **********************************

/**
* @~english
* @brief Set an offset to the display
*
* @param num Number of the element on which the cursor will be set
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt den Cursor an eine bestimmte Position
*
* @param num Nummber der Zeichnposition an die der Cursor gesetzt werden soll
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::offset(int num){
	__u8 tmp;
	if(!init_flag){
		char init_str[2] = " ";
		init();
		print(init_str);
	}
	if (num >= 0 && num < 16){
		tmp = num + 128;
	}
	else if (num >= 16 && num < 32){
		tmp = num - 16 + 192;
	}
	else {
		error_flag = true;
		return -1;
	}
	if (spi.send(&tmp, 1) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//*************** clear() ****************************************

/**
* @~english
* @brief clear the display
*
* @return 1 by success, -1 by failure
*
* @~german
* @brief Löscht den Inhalt des Displays.
*
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::clear(){
	__u8 clear_cmd = 0x01;
	if(!init_flag){
		init();
	}
	if (spi.send(&clear_cmd, 1) < 0){
		error_flag = true;
		return -1;
	}
	return 1;
}


//**************** returnHome() ************************************

/**
* @~english
* @brief Reset the Display
*
* @return 1 by success, -1 by failure
*
* @~german
* @brief Resetet das Display.
*
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::returnHome(){
	__u8 return_cmd = 0x02;
	if(!init_flag){
		init();
	}
	if (spi.send(&return_cmd, 1) < 0){
		error_flag = true;
		return -1;
	}
	return 1;
}	


//**************** shift() *****************************************

/**
* @~english
* @brief Shift the whole display
*
* @param num Number of shifts, positive: right, negativ: left
* @return 1 by success, -1 by failure
*
* @~german
* @brief Verschiebt das ganze Display
*
* @param num Anzahl der zu verschiebenden Positionen, positiv: rechts, negativ: left
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::shift(int num){
	__u8 shift_cmd;
	if(!init_flag){
		init();
	}
	if (num > 0){
		shift_cmd = 0x1C;
		for (int i=0; i < num; i++){
			if(spi.send(&shift_cmd, 1) < 0){
				error_flag = true;
				return -1;
			}
		}
	}
	else if (num < 0){
		shift_cmd = 0x18;
		for (int i=0; i > num; i--){
			if(spi.send(&shift_cmd, 1) < 0){
				error_flag = true;
				return -1;
			}
		}
	}
	else {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//*********************** controlDisplay() **********************************

/**
* @~english
* @brief Set display parameters
*
* @param power switch display on(1) or off(0)  (not the chontrollerchip)
* @param cursor switch cursor on(1) or off(0)
* @param blink switch the vlinking of the cursor on(1) or off(0)
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt Displayparameter.
*
* @param power Display an(1) oder aus(0) schalten (nicht den Displaycontroller)
* @param cursor Schatet den Cursor an(1) oder aus(0)
* @param blink Schaltet das Blinken des Cursors an(1) oder aus(0)
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_dogm::controlDisplay(int power, int cursor, int blink) {
	__u8 display_cmd = 0x08;
	if(!init_flag){
		init();
	}
	if (power == 1) {
		display_cmd += 0x04;
	}
	if (cursor == 1) {
		display_cmd += 0x02;
	}
	if (blink == 1) {
		display_cmd += 0x01;
	}
	if (spi.send(&display_cmd, 1) < 0){
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}
//*******************************************************************
//Class for accessing the LM75 IC via I2C
//*******************************************************************

//------------------Konstruktor------------------
/** @~english 
* @brief Sets the error_flag to "false" and the standard i2c Address to 0x4f
*
* @~german 
* @brief Setzt das error_flag auf "false" und die Standard i2c Adresse auf 0x4f
*
*/
gnublin_module_lm75::gnublin_module_lm75()
{
	error_flag=false;
	setAddress(0x4f);
}


//-------------get Error Message-------------
/** @~english 
* @brief Get the last Error Message.
*
* This function returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_module_lm75::getErrorMessage(){
	return ErrorMessage.c_str();
}

//-------------------------------Fail-------------------------------
/** @~english 
* @brief returns the error flag to check if the last operation went wrong
*
* @return error_flag as boolean
*
* @~german 
* @brief Gibt das error_flag zurück um zu überprüfen ob die vorangegangene Operation einen Fehler auweist
*
* @return error_flag als bool
*/
bool gnublin_module_lm75::fail(){
	return error_flag;
}

//-------------set Address-------------
/** @~english 
* @brief Set the i2c slave address 
*
* With this function you can set the individual I2C Slave-Address.
* @param Address new I2C slave Address
*
* @~german 
* @brief Setzt die i2c slave Adresse
*
* Mit dieser Funktion kann die individuelle I2C Slave-Adresse gesetzt werden.
* @param Address neue I2C slave Adresse
*/
void gnublin_module_lm75::setAddress(int Address){
	i2c.setAddress(Address);
}


//-------------------set devicefile----------------
/** @~english
* @brief set i2c the device file. default is "/dev/i2c-1"
*
* This function sets the devicefile you want to access. by default "/dev/i2c-1" is set.
* @param filename path to the devicefile e.g. "/dev/i2c-0"
*
* @~german
* @brief setzt die I2C Device Datei. Standard ist die "/dev/i2c-1"
*
* Diese Funktion setzt die Geräte Datei, auf die man zugreifen möchte. Standardmäßig ist bereits "/dev/i2c-1" gesetzt.
* @param filename Dateipfad zur Geräte Datei, z.B. "/dev/i2c-0"
*/
void gnublin_module_lm75::setDevicefile(std::string filename){
	i2c.setDevicefile(filename);
}


//-----------------------------------get Temp-----------------------------------
/** @~english
* @brief reads the raw data via i2c from the LM75 chip and calculates the temperature
*
* @return Returns the Temperature, 0 at failure (check with fail() and getErrorMessage())
*
* @~german
* @brief liest die Roh-Daten aus dem LM75 und berechnet die Temperatur
*
* @return Temperatur, im Fehlerfall 0 (überprüfen mit fail() und getErrorMessage())
*/
int gnublin_module_lm75::getTemp(){
	short value=0;
	int temp;	
	unsigned char rx_buf[2];
	error_flag=false;

	if(i2c.receive(0x00, rx_buf, 2)>0){
		// rx_buf[0] = MSByte
		// rx_buf[1] = LSByte
		// save the MSB
		value = rx_buf[0];
		// make space for the LSB
		value<<=8;
		// save the LSB
		value |= rx_buf[1];
		// Bit 0-4 isn't used in the LM75, so shift right 5 times
		value>>=5;

		//check if temperature is negative
		if(rx_buf[0] & 0x80)
		{
			value = value|0xF800;
			value =~value +1;
			temp = value*(-0.125);
			return temp;
		}
		else { //temperature is positive
			temp = value*0.125;
			return temp;
		}


	}
	else{
		error_flag=true;
		ErrorMessage = "Error i2c receive\n";
		return 0;	
	}
}


//--------------------------------get Temp float--------------------------------
/** @~english
* @brief reads the raw data via i2c from the LM75 chip and calculates the temperature
*
* @return Returns the Temperature as float, 0 at failure (check with fail() and getErrorMessage())
*
* @~german
* @brief liest die Roh-Daten aus dem LM75 und berechnet die Temperatur
*
* @return Temperatur als Fließkommazahl, im Fehlerfall 0 (überprüfen mit fail() und getErrorMessage())
*/
float gnublin_module_lm75::getTempFloat(){
	short value=0;
	float temp;	
	unsigned char rx_buf[2];
	error_flag=false;

	if(i2c.receive(0x00, rx_buf, 2)>0){
		// rx_buf[0] = MSByte
		// rx_buf[1] = LSByte
		// save the MSB
		value = rx_buf[0];
		// make space for the LSB
		value<<=8;
		// save the LSB
		value |= rx_buf[1];
		// Bit 0-4 isn't used in the LM75, so shift right 5 times
		value>>=5;

		//check if temperature is negative
		if(rx_buf[0] & 0x80)
		{
			value = value|0xF800;
			value =~value +1;
			temp = value*(-0.125);
			return temp;
		}
		else { //temperature is positive
			temp = value*0.125;
			return temp;
		}
		
	}
	else{	
		error_flag=true;
		ErrorMessage="Error i2c receive\n";
		return 0;
	}
		
}


//--------------------------------get Value---------------------------------
/** @~english
* @brief reads the raw data via i2c from the LM75 chip and shift the bits correctly
*
* @return Returns raw value, already shifted. 0 at failure (check with fail() and getErrorMessage())
*
* @~german
* @brief liest die Roh-Daten aus dem LM75 und schiebt die Bits in die richtige Reihenfolge
*
* @return Rohwert, im Fehlerfall 0 (überprüfen mit fail() und getErrorMessage())
*/
short gnublin_module_lm75::getValue(){
	short value=0;
	unsigned char rx_buf[2];
	error_flag=false;
	if(i2c.receive(0x00, rx_buf, 2)>0){
		
		// rx_buf[0] = MSByte
		// rx_buf[1] = LSByte
		// save the MSB
		value = rx_buf[0];
		// make space for the LSB
		value<<=8;
		// save the LSB
		value |= rx_buf[1];
		// Bit 0-4 isn't used in the LM75, so shift right 5 times
		value>>=5;
		return value;
		
	}
	else{
		error_flag=true;
		ErrorMessage="Error i2c receive\n";
		return 0;
	}
}


//*****************************************************************************
// Class for accesing GNUBLIN Module-ADC / ADS7830
//*****************************************************************************

/**
* @~english
* @brief Set the default I2C Address and reference to intern
*
* Default I2C Address: 0x48
*
* @~german
* @brief Setzt die standard I2C Adresse und die Referenzspannung auf intern
*
* Standard I2C Addresse: 0x48
*/
gnublin_module_adc::gnublin_module_adc() {
	i2c.setAddress(0x48);
	referenceValue = 2500;
	reference_flag = IN;
	error_flag = false;
}

//-------------get Error Message-------------

/**
* @~english
* @brief Returns the ErrorMessage of the previous error if one exist.
*
* @return ErrorMessage as C-String
*
* @~german
* @brief Gibt die Fehlernachricht des zuvor aufgetretenen Fehlers zurück, wenn weine exisitert.
*
* @return ErrorMessage als C-String
*/
const char *gnublin_module_adc::getErrorMessage(){
	return ErrorMessage.c_str();
}


//------------ fail() -----------------------

/**
* @~english
* @brief Returns the errorflag to detect error in the previous called method.
*
* @return error_flag
*
* @~german
* @brief Gibt das errorflag zurück, um Fehler in der zuvor aufgerugfenen Methode zu erkennen.
*
* @return error_flag
*/
bool gnublin_module_adc::fail() {
	return error_flag;
}


//-------------setAddress-------------
// set the slave address
// parameters:		[int]address	i2c slave Address
// return:		NONE
/**
* @~english
* @brief Set the I2C Address
*
* @param address I2C Address
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt die I2C Adresse.
*
* @param address I2C Adresse
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_adc::setAddress(int address) {
	i2c.setAddress(address);
	if (i2c.fail()) {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//-------------------setDevicefile----------------
// set the i2c device file. default is "/dev/i2c-1"
// parameters:		[string]filename	path to the dev file
// return:		NONE
/**
* @~english
* @brief Set a custom I2C devicefile
*
* default devicefile: "/dev/i2c-1"
* @param filename path of the devicefile
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt ein benutzerdefiniertes Devicefile.
*
* Standard Devicefile ist "/dev/i2c-1"
* @param filename Pfad zum Devicefile
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_adc::setDevicefile(std::string filename) {
	i2c.setDevicefile(filename);
	if (i2c.fail()) {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}

//-------------------setReference() ----------------

/**
* @~english
* @brief Set the reverencevoltage to intern or extern
*
* @param value IN (1) for intern (2.5V), OUT (0) for extern (3.3V)
* @return 1 by success, -1 by failure
*
* @~german
* @brief Setzt die referenzspannung auf intern oder extern.
*
* @param value IN (1) für intern (2,5V), OUT (0) für extern (3,3V)
* @return 1 bei Erfolg, -1 im Fehlerfall
*/
int gnublin_module_adc::setReference(int value) {
	if (value == 0) {
		referenceValue = 3300;
		reference_flag = 0;
	}
	else if (value == 1) {
		referenceValue = 2500;
		reference_flag = 1;
	}
	else {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return 1;
}


//---------------------- getValue() -----------------------

/**
* @~english
* @brief Get a value of an ADC channel in reference to GND
*
* @param channel Number of the ADC-channel (1-8)
* @return value
*
* @~german
* @brief Liefert den Wert eine ADC Ports bezogen zu GND
*
* @param channel Nummer des ADC-Ports (1-8)
* @return Wert
*/
int gnublin_module_adc::getValue(int channel) {
	int command;
	unsigned char value[1];
	
	if (reference_flag == 0){
		switch (channel) {
			case 1: command = 0x87; break;
			case 2: command = 0xC7; break;
			case 3: command = 0x97; break;
			case 4: command = 0xD7; break;
			case 5: command = 0xA7; break;
			case 6: command = 0xE7; break;
			case 7: command = 0xB7; break;
			case 8: command = 0xF7; break;
			default: error_flag = true; return -1; break;
		}
	}
	else {
		switch (channel) {
			case 1: command = 0x8F; break;
			case 2: command = 0xCF; break;
			case 3: command = 0x9F; break;
			case 4: command = 0xDF; break;
			case 5: command = 0xAF; break;
			case 6: command = 0xEF; break;
			case 7: command = 0xBF; break;
			case 8: command = 0xFF; break;
			default: error_flag = true; return -1; break;
		}
	}
	
	i2c.send(command);
	if (i2c.fail()) {
		error_flag = true;
		return -1;
	}

	i2c.receive(value, 1);
	if (i2c.fail()) {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return value[0];	
}

/**
* @~english
* @brief Get a value of an ADC channel in reference to a second ADC channel (differential)
*
* This combinations are possible<br>
* 1 - 2<br>
* 3 - 4<br>
* 5 - 6<br>
* 7 - 8<br>
* 2 - 1<br>
* 4 - 3<br>
* 6 - 5<br>
* 8 - 7<br>
* @param channel1 Number of the first ADC-channel 
* @param channel2 Number of the second ADC-channel
* @return value
*
* @~german
* @brief Liefert den Wert eines ADC Ports bezogen auf einen zweiten ADC Port (differentielle Messung)
*
* Dies Kombinationen sind möglich:<br>
* 1 - 2<br>
* 3 - 4<br>
* 5 - 6<br>
* 7 - 8<br>
* 2 - 1<br>
* 4 - 3<br>
* 6 - 5<br>
* 8 - 7<br>
* @param channel1 Nummer des ersten ADC-Ports
* @param channel2 Nummer des zweiten ADC-Ports
* @return Wert
*/
int gnublin_module_adc::getValue(int channel1, int channel2) {
	int command = -1;
	unsigned char value[1];
	
	for (int i = 1; i<9; i += 2 ) {
		if (channel1 == i && channel2 == (i+1)){
			switch(i) {
				case 1: command = 0x00; break;
				case 3: command = 0x10; break;
				case 5: command = 0x20; break;
				case 7: command = 0x30; break;
			}
		}
		if (channel2 == i && channel1 == (i+1)) {
			switch(i) {
				case 1: command = 0x40; break;
				case 3: command = 0x50; break;
				case 5: command = 0x60; break;
				case 7: command = 0x70; break;
			}
		}
	}
	if (command == -1){
		error_flag = true;
		return -1;
	}
	if (reference_flag == 0) {
		command += 0x7;
	}
	else {
		command += 0xF;
	}
	i2c.send(command);
	if (i2c.fail()) {
		error_flag = true;
		return -1;
	}

	i2c.receive(value, 1);
	if (i2c.fail()) {
		error_flag = true;
		return -1;
	}
	error_flag = false;
	return value[0];
}


//---------------------- getVoltage() -----------------------
// get ADC Value of channel in reference to GND
// parameters:		[int] channel		ADC-Port
// return:		[int] voltage		voltage of ADC-channel in mV
/**
* @~english
* @brief Get the voltage of an ADC channel in reference to GND in mV
*
* @param channel Number of the ADC-channel (1-8)
* @return value in mV
*
* @~german
* @brief Liefert den Wert eine ADC Ports bezogen zu GND in mV
*
* @param channel Nummer des ADC-Ports (1-8)
* @return Wert in mV
*/
int gnublin_module_adc::getVoltage(int channel) {
	error_flag = false;
	int voltage;
	int value = getValue(channel);
	if (error_flag) {
		return -1;
	}
	
	voltage = value * referenceValue / 255;
	return voltage;
}

/**
* @~english
* @brief Get the voltage of an ADC channel in reference to a second ADC channel in mV (differential)
*
* This combinations are possible<br>
* 1 - 2<br>
* 3 - 4<br>
* 5 - 6<br>
* 7 - 8<br>
* 2 - 1<br>
* 4 - 3<br>
* 6 - 5<br>
* 8 - 7<br>
* @param channel1 Number of the first ADC-channel 
* @param channel2 Number of the second ADC-channel
* @return voltage in mV
*
* @~german
* @brief Liefert den Wert eines ADC Ports bezogen auf einen zweiten ADC Port in mV (differentielle Messung)
*
* Dies Kombinationen sind möglich:<br>
* 1 - 2<br>
* 3 - 4<br>
* 5 - 6<br>
* 7 - 8<br>
* 2 - 1<br>
* 4 - 3<br>
* 6 - 5<br>
* 8 - 7<br>
* @param channel1 Nummer des ersten ADC-Ports
* @param channel2 Nummer des zweiten ADC-Ports
* @return Wert in mV
*/
int gnublin_module_adc::getVoltage(int channel1, int channel2) {
	error_flag = false;
	int voltage;
	int value = getValue(channel1, channel2);
	if (error_flag) {
		return -1;
	}
	
	voltage = value * referenceValue / 255;
	return voltage;
}

//*******************************************************************
//Class for accessing GNUBLIN Module-Portexpander or any PCA9555
//*******************************************************************

//------------------Konstruktor------------------
/** @~english 
* @brief Set standard i2c address 0x20, set ErrorFlag false
*
* @~german 
* @brief Setze standard i2c Adresse 0x20 und setze das ErrorFlag auf false.
*
*/
gnublin_module_pca9555::gnublin_module_pca9555() 
{
	error_flag=false;
	setAddress(0x20);
}


//-------------get Error Message-------------
/** @~english 
* @brief Get the last Error Message.
*
* This function returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_module_pca9555::getErrorMessage(){
	return ErrorMessage.c_str();
}

//-------------------------------Fail-------------------------------
/** @~english 
* @brief returns the error flag to check if the last operation went wrong
*
* @return error_flag as boolean
*
* @~german 
* @brief Gibt das error_flag zurück um zu überprüfen ob die vorangegangene Operation einen Fehler auweist
*
* @return error_flag als bool
*/
bool gnublin_module_pca9555::fail(){
	return error_flag;
}

//-------------set Address-------------
/** @~english 
* @brief Set the i2c slave address 
*
* With this function you can set the individual I2C Slave-Address of the PCA9555.
* @param Address new I2C slave Address
*
* @~german 
* @brief Setzt die i2c slave Adresse
*
* Mit dieser Funktion kann die individuelle I2C Slave-Adresse des PCA9555 gesetzt werden.
* @param Address neue I2C slave Adresse
*/
void gnublin_module_pca9555::setAddress(int Address){
	i2c.setAddress(Address);
}


//-------------------set devicefile----------------
/** @~english
* @brief set i2c the device file. default is "/dev/i2c-1"
*
* This function sets the devicefile you want to access. by default "/dev/i2c-1" is set.
* @param filename path to the devicefile e.g. "/dev/i2c-0"
*
* @~german
* @brief setzt die I2C Device Datei. Standard ist die "/dev/i2c-1"
*
* Diese Funktion setzt die Geräte Datei, auf die man zugreifen möchte. Standardmäßig ist bereits "/dev/i2c-1" gesetzt.
* @param filename Dateipfad zur Geräte Datei, z.B. "/dev/i2c-0"
*/
void gnublin_module_pca9555::setDevicefile(std::string filename){
	i2c.setDevicefile(filename);
}

//-----------------------------------Pin Mode-----------------------------------
/** @~english
* @brief Controls the pin mode (INPUT/OUTPUT)
*
* With this Function you can set a single pin of the PCA9555 wether as input or output.
* @param pin Number of the pin (0-15)
* @param direction INPUT or OUTPUT
* @return success: 1, failure: -1
*
* @~german
* @brief Setzt den Pin Modus (Eingang/Ausgang)
*
* Mit dieser Funktion kann man einen einzelnen Pin entweder als Eingang oder als Ausgang setzen.
* @param pin Nummer des Pins (0-15)
* @param direction INPUT (Eingang) oder OUTPUT (Ausgang)
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_module_pca9555::pinMode(int pin, std::string direction){
	error_flag=false;
	unsigned char TxBuf[1];
	unsigned char RxBuf[1];

	if (pin < 0 || pin > 15){
		error_flag=true;
		ErrorMessage="Pin Number is not between 0-15";
		return -1;
	}

	if(pin >= 0 && pin <= 7){ // Port 0

			TxBuf[0]=pow(2, pin); //convert pin into its binary form e. g. Pin 3 = 8

			if (i2c.receive(0x06, RxBuf, 1)>0){ //read the current state

				if (direction=="out"){
					TxBuf[0]=RxBuf[0] & ~TxBuf[0]; // at output you have to invert the pin you want to set und AND it to change only the pin 
					if(i2c.send(0x06, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}
									
				}
				else if (direction=="in"){
					TxBuf[0]=RxBuf[0] | TxBuf[0]; // at input you just have to do a OR
					if(i2c.send(0x06, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}
				}
				else{
					error_flag=true;
					ErrorMessage="direction != IN/OUTPUT";				
					return -1;			
				}
			}
			else {
			error_flag=true;
			ErrorMessage="i2c.receive Error";
			return -1;
			}
	}
	else if(pin >= 8 && pin <= 15){ // Port 1

			TxBuf[0]=pow(2, (pin-8)); //convert pin into its binary form e. g. Pin 3 = 8

			if(i2c.receive(0x07, RxBuf, 1)>0){ //read the current state
			
				if (direction=="out"){
					TxBuf[0]=RxBuf[0] & ~TxBuf[0];
					if(i2c.send(0x07, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}					
				}
				else if (direction=="in"){
					TxBuf[0]=RxBuf[0] | TxBuf[0];
					if(i2c.send(0x07, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}
				}
				else{
					error_flag=true;
					ErrorMessage="direction != IN/OUTPUT";				
					return -1;			
				}
			}
			else {
			error_flag=true;
			ErrorMessage="i2c.receive Error";
			return -1;
			}
	}
	else return -1;
}


//-----------------------------------Port Mode-----------------------------------
/** @~english
* @brief Controls the port mode (INPUT/OUTPUT)
*
* With this Function you can set a whole port of the PCA9555 wether as input or output.
* @param port Number of the port (0-1)
* @param direction INPUT or OUTPUT
* @return success: 1, failure: -1
*
* @~german
* @brief Setzt den Port Modus (Eingang/Ausgang)
*
* Mit dieser Funktion kann man einen ganzen Port entweder als Eingang oder als Ausgang setzen.
* @param port Nummer des Ports (0-1)
* @param direction INPUT (Eingang) oder OUTPUT (Ausgang)
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_module_pca9555::portMode(int port, std::string direction){
	error_flag=false;
	unsigned char TxBuf[1];

	if (port < 0 || port > 1){
		error_flag=true;
		ErrorMessage="Port Number is not between 0-1";
		return -1;
	}

	if(port == 0){ // Port 0

			if (direction=="out"){
				TxBuf[0]=0x00;
				if(i2c.send(0x06, TxBuf, 1)>0){
				return 1;
				}
				else {
					error_flag=true;
					ErrorMessage="i2c.send Error";
					return -1;
				}
								
			}
			else if (direction=="in"){
				TxBuf[0]=0xff;
				if(i2c.send(0x06, TxBuf, 1)>0){
				return 1;
				}
				else {
					error_flag=true;
					ErrorMessage="i2c.send Error";
					return -1;
				}
			}
			else{
				error_flag=true;
				ErrorMessage="direction != IN/OUTPUT";				
				return -1;			
			}
	}
	else if(port==1){ // Port 1
		
			if (direction=="out"){
				TxBuf[0]=0x00;
				if(i2c.send(0x07, TxBuf, 1)>0){
				return 1;
				}
				else {
					error_flag=true;
					ErrorMessage="i2c.send Error";
					return -1;
				}					
			}
			else if (direction=="in"){
				TxBuf[0]=0xff;
				if(i2c.send(0x07, TxBuf, 1)>0){
				return 1;
				}
				else {
					error_flag=true;
					ErrorMessage="i2c.send Error";
					return -1;
				}
			}
			else{
				error_flag=true;
				ErrorMessage="direction != IN/OUTPUT";				
				return -1;			
			}
	}
	else return -1;
}


//-----------------------------------digital Write-----------------------------------
/** @~english
* @brief sets the level of an output pin (HIGH/LOW)
*
* With this Function you can set the logical level of an output pin to wether HIGH or LOW.
* @param pin Number of the pin (0-15)
* @param value HIGH (1) or LOW (0)
* @return success: 1, failure: -1
*
* @~german
* @brief Setzt den Pegel eines Ausgangs (HIGH/LOW)
*
* Mit dieser Funktion kann man den Pegel eines Ausgangs entweder auf HIGH oder auf LOW setzen.
* @param pin Nummer des Pins (0-15)
* @param value HIGH (1) oder LOW (0) 
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_module_pca9555::digitalWrite(int pin, int value){
	error_flag=false;
	unsigned char TxBuf[1];
	unsigned char RxBuf[1];

	if (pin < 0 || pin > 15){
		error_flag=true;
		ErrorMessage="Pin Number is not between 0-15";
		return -1;
	}


	if(pin >= 0 && pin <= 7){ // Port 0

			TxBuf[0]=pow(2, pin); //convert pin into its binary form e. g. Pin 3 = 8

			if (i2c.receive(0x02, RxBuf, 1)>0){ //read the current state

				if (value==0){
					TxBuf[0]=(RxBuf[0] & ~TxBuf[0]); // at low you have to invert the pin you want to set and do a AND to change only the pin you want 
					if(i2c.send(0x02, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}
									
				}
				else if (value==1){
					TxBuf[0]=(RxBuf[0] | TxBuf[0]); // at high you just have to do a OR
					if(i2c.send(0x02, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}
				}
				else{
					error_flag=true;
					ErrorMessage="value != 1/0";				
					return -1;			
				}
			}
			else {
			error_flag=true;
			ErrorMessage="i2c.receive Error";
			return -1;
			}
	}
	else if(pin >= 8 && pin <= 15){ // Port 1

			TxBuf[0]=pow(2, (pin-8)); //convert pin into its binary form e. g. Pin 3 = 8

			if(i2c.receive(0x03, RxBuf, 1)>0){ //read the current state
				if (value==0){
					
					TxBuf[0]=RxBuf[0] & ~TxBuf[0];
					if(i2c.send(0x03, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}					
				}
				else if (value==1){
					TxBuf[0]=RxBuf[0] | TxBuf[0];
					if(i2c.send(0x03, TxBuf, 1)>0){
					return 1;
					}
					else {
						error_flag=true;
						ErrorMessage="i2c.send Error";
						return -1;
					}
				}
				else{
					error_flag=true;
					ErrorMessage="value != HIGH/LOW";				
					return -1;			
				}
			}
			else {
			error_flag=true;
			ErrorMessage="i2c.receive Error";
			return -1;
			}
	}
	else return -1;
}

//-----------------------------------write Port-----------------------------------
/** @~english
* @brief  Writes one byte to a complete port
*
* With this Function you can write a whole byte to a whole port of the PCA9555.
* @param port Number of the port (0-1)
* @param value The Byte you want to write.  e.g. 0x12 to write 00010010 
* @return success: 1, failure: -1
*
* @~german
* @brief Schreibt ein Byte an einen kompletten Port
*
* Mit dieser Funktion kann man ein komplettes Byte an einen ganzen Port des PCA9555 schreiben.
* @param port Nummer des Ports (0-1)
* @param value Das Byte, dass man schreiben möchte. z.B. 0x12 um 00010010 zu schreiben. 
* @return Erfolg: 1, Misserfolg: -1
*/
int gnublin_module_pca9555::writePort(int port, unsigned char value){
	error_flag=false;
	unsigned char buffer[1];
	buffer[0]=value;

	if(port==0){ // Port 0
		if(i2c.send(0x02, buffer, 1)>0){
			return 1;
		}
		else {
			error_flag=true;
			ErrorMessage="i2c.send Error";
			return -1;
		}
	}
	else if(port==1){
		if(i2c.send(0x03, buffer, 1)>0){
			return 1;
		}
		else {
			error_flag=true;
			ErrorMessage="i2c.send Error";
			return -1;
		}
	}		
	else{
		error_flag=true;
		ErrorMessage="Pin Number is not between 0-1";
		return -1;		
	}
	
}

//-----------------------------------digital read-----------------------------------
/** @~english
* @brief reads the state of an input pin and returns it
*
* With this function you can read the level of an input pin.
* @param pin Number of the pin (0-15) you want to read from
* @return 0/1 logical level of the pin, failure: -1
*
* @~german
* @brief Liest den Zustand eins Eingang-Pins und gibt ihn zurück
*
* Mit dieser Funktion kann man den Zustand eines Pins auslesen.
* @param pin Nummer des Pins (0-15) von dem man lesen will.
* @return 0/1 logischer Pegel des Pins, Misserfolg: -1
*/
int gnublin_module_pca9555::digitalRead(int pin) {
	error_flag=false;
	unsigned char RxBuf[1];

	if (pin < 0 || pin > 15){
		error_flag=true;
		ErrorMessage="Pin Number is not between 0-15\n";
		return -1;
	}
	
	if(pin >= 0 && pin <= 7){ // Port 0		
		if(i2c.receive(0x00, RxBuf, 1)>0){

				RxBuf[0]<<=(7-pin); // MSB is now the pin you want to read from
				RxBuf[0]&=128; // set all bits to 0 except the MSB
		
				if(RxBuf[0]==0){
					return 0;
				}
				else if (RxBuf[0]==128){
					return 1;
				}
				else{
					error_flag=true;
					ErrorMessage="bitshift failed\n";
					return -1;
				}
		}
		else{
			error_flag=true;
			ErrorMessage="i2c.receive Error";
			return -1;
		}
	}
	else if(pin >= 8 && pin <= 15){ // Port 1
		if(i2c.receive(0x01, RxBuf, 1)>0){

				RxBuf[0]<<=(15-pin); // MSB is now the pin you want to read from
				RxBuf[0]&=128;	// set all bits to 0 except the MSB	
		
				if(RxBuf[0]==0){
					return 0;
				}
				else if (RxBuf[0]==128){
					return 1;
				}
				else{
					error_flag=true;
					ErrorMessage="bitshift failed\n";
					return -1;
				}
		}
		else{
			error_flag=true;
			ErrorMessage="i2c.receive Error";
			return -1;
		}
	}
	error_flag=true;
	ErrorMessage="something went wrong";
	return -1;
}


//****************************************************************************
// Class for easy use of the GNUBLIN Module-Relay
//****************************************************************************


//------------------Konstruktor------------------
/** @~english 
* @brief Set standards.
*
* Set standard i2c address 0x20, set ErrorFlag false
* @~german 
* @brief Setze Standartwerte.
*
* Setze standard i2c Adresse 0x20 und setze das ErrorFlag auf false.
*/
gnublin_module_relay::gnublin_module_relay() {
	error_flag=false;
	setAddress(0x20);
}


//-------------getErrorMessage-------------
/** @~english 
* @brief Get the last Error Message.
*
* This Funktion returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_module_relay::getErrorMessage(){
	return ErrorMessage.c_str();
}

//-------------------------------fail-------------------------------
/** @~english 
* @brief Returns the error flag. 
*
* If something went wrong, the flag is true.
* @return bool error_flag
*
* @~german 
* @brief Gibt das Error Flag zurück.
*
* Falls das Error Flag in der Klasse gesetzt wurde, wird true zurück gegeben, anderenfalls false.
* @return bool error_flag
*/
bool gnublin_module_relay::fail(){
	return error_flag;
}

//-------------setAddress-------------
/** @~english 
* @brief Set the slave address 
*
* With this function you can set the individual I2C Slave-Address of the module.
* @param Address new I2C slave Address
*
* @~german 
* @brief Setzt Slave Adresse.
*
* Mit dieser Funktion kann die individuelle I2C Slave-Adresse des Moduls gesetzt werden.
* @param Address neue I2C slave Adresse
*/
void gnublin_module_relay::setAddress(int Address){
	pca9555.setAddress(Address);
}


//-------------setDevicefile-------------
/** @~english 
* @brief Set devicefile.
*
* With this function you can change the I2C device file. Default is "/dev/i2c-1"
* @param filename path to the I2C device file
*
* @~german 
* @brief Setzt Device Datei.
*
* Mit dieser Funktion kann die I2C Gerätedatei geändert werden. Standartmäßig wird "/dev/i2c-1" benutzt.
* @param filename Pfad zur I2C Gerätedatei
*/
void gnublin_module_relay::setDevicefile(std::string filename){
	pca9555.setDevicefile(filename);
}

//-------------------switch Pin----------------
/** @~english 
* @brief Switch pin.
*
* This function sets the given relay to the given value.
* @param pin Number of the relay to switch (1-8)
* @param value close (1) or open (0) the relay (ON, OFF)
* @return success: 1, failure: -1
*
* @~german 
* @brief Schalte Pin.
*
* Schalten der einzelnen Relays. 
* @param pin Nummer des anzusteuernden Relays (1-8)
* @param value schließen (1) oder öffnen (0) des Relays (ON, OFF)
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_relay::switchPin(int pin, int value) {
	error_flag=false;

	if (pin < 1 || pin > 8) {
		error_flag=true;
		ErrorMessage="pin is not between 1-8!\n";
		return -1;
	}
	if (pca9555.pinMode((pin-1), OUTPUT) < 0){
		error_flag=true;
		ErrorMessage=pca9555.getErrorMessage(); //"pca9555.pinMode failed! Address correct?\n";
		return -1;
	}
	if (pca9555.digitalWrite((pin-1), value) < 0) {
		error_flag=true;
		ErrorMessage="pca9555.digitalWrite failed! Address correct?\n";		
		return -1;
	}
	return 1;
}



//*******************************************************************
//Class for accessing GNUBLIN Module-step
//*******************************************************************

//-------------gnublin_module_step-------------
/** @~english 
* @brief Set irun and vmax to the default values (irun = 15, vmax = 8).
*
* @~german 
* @brief Setzt die Standartwerte für irun und vmax (irun = 15, vmax = 8).
*
*/
gnublin_module_step::gnublin_module_step()
{
	irun = 15;
	vmax = 8;
}

//-------------getErrorMessage-------------
/** @~english 
* @brief Get the last Error Message.
*
* This Funktion returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_module_step::getErrorMessage(){
	return ErrorMessage.c_str();
}

//-------------setAddress-------------
/** @~english 
* @brief Set the slave address 
*
* With this function you can set the individual I2C Slave-Address of the module.
* @param Address new I2C slave Address
*
* @~german 
* @brief Setzt Slave Adresse.
*
* Mit dieser Funktion kann die individuelle I2C Slave-Adresse des Moduls gesetzt werden.
* @param Address neue I2C slave Adresse
*/
void gnublin_module_step::setAddress(int Address){
	i2c.setAddress(Address);
}

//-------------setDevicefile-------------
/** @~english 
* @brief Set devicefile.
*
* With this function you can change the I2C device file. Default is "/dev/i2c-1"
* @param filename path to the I2C device file
*
* @~german 
* @brief Setzt Device Datei.
*
* Mit dieser Funktion kann die I2C Gerätedatei geändert werden. Standartmäßig wird "/dev/i2c-1" benutzt.
* @param filename Pfad zur I2C Gerätedatei
*/
void gnublin_module_step::setDevicefile(std::string filename){
	i2c.setDevicefile(filename);
}

//-------------setIrun-------------
/** @~english 
* @brief Set Irun.
*
* This Funktion sets the new Irun value.
* @param newIrun values from 0 to 15 are possible
* @return success: 1, failure: -1
*
* @~german 
* @brief Setze Irun.
*
* Diese Funktion setzt den neuen Irun Wert
* @param newIrun Werte von 0 bis 15
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::setIrun(unsigned int newIrun){
	if(newIrun <= 15){
		irun=newIrun;
		return 1;
	}
	else return -1;
}

//-------------setIhold-------------
/** @~english 
* @brief Set Ihold.
*
* This Funktion sets the new Ihold value.
* @param newIhold values from 0 to 15 are possible
* @return success: 1, failure: -1
*
* @~german 
* @brief Setze Ihold
*
* Diese Funktion setzt den neuen Ihold Wert
* @param newIhold Werte von 0 bis 15
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::setIhold(unsigned int newIhold){
	if(newIhold <= 15){
		ihold=newIhold;
		return 1;
	}
	else return -1;
}

//-------------setVmax-------------
/** @~english 
* @brief Set Vmax.
*
* This Funktion sets the new Vmax value.
* @param newVmax values from 0 to 15 are possible
* @return success: 1, failure: -1
*
* @~german 
* @brief Setze Vmax
*
* Diese Funktion setzt den neuen Vmax Wert
* @param newVmax Werte von 0 bis 15
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::setVmax(unsigned int newVmax){
	if(newVmax <= 15){
		vmax=newVmax;
		return 1;
	}
	else return -1;
}

//-------------setVmin-------------
/** @~english 
* @brief Set Vmin.
*
* This Funktion sets the new Vmin value.
* @param newVmin values from 0 to 15 are possible
* @return success: 1, failure: -1
*
* @~german 
* @brief Setze Vmin
*
* Diese Funktion setzt den neuen Vmin Wert
* @param newVmin Werte von 0 bis 15
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::setVmin(unsigned int newVmin){
	if(newVmin <= 15){
		vmin=newVmin;
		return 1;
	}
	else return -1;
}

//-------------writeTMC-------------
/** @~english 
* @brief Write to TMC.
*
* This funktion sends the amount of bytes (num) of the char array TxBuf to the TMC222 chip on the module-step.
* @param TxBuf Send buffer
* @param num amount of Bytes
* @return success: 1, failure: -1
*
* @~german 
* @brief Sende an TMC.
*
* Diese Funktion sendet die Menge (num) an Bytes des Char Array TxBuf an den TMC222 Chip, welcher sich auf dem Moudule-step befindet.
* @param TxBuf Sende Puffer
* @param num Anzahl der zu sendenden Bytes
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::writeTMC(unsigned char *TxBuf, int num){
	if(!i2c.send(TxBuf, num)){
	    return -1;
   	}
	else return 1;
}

//-------------readTMC-------------
/** @~english 
* @brief Read from TMC.
*
* This funktion reads the amount of bytes (num) of the TMC222 chip on the module-step and write it to the char array RxBuf.
* @param RxBuf Receive buffer
* @param num amount of Bytes
* @return success: 1, failure: -1
*
* @~german 
* @brief Sende an TMC.
*
* Diese Funktion ließt die Menge (num) an Bytes vom TMC222 Chip, welcher sich auf dem Moudule-step befindet und übergibt das Ergebnis im Char Array RxBuf.
* @param RxBuf Empfangs Puffer
* @param num Anzahl der zu lesenden Bytes
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::readTMC(unsigned char *RxBuf, int num){
   	if(!i2c.receive(RxBuf, num)){
       	return -1;
    }
	else return 1;	
}

//-------------burnNewAddress-------------
/** @~english 
* @brief Burn a new I2C slave Address.
*
* This function calculates the needet bits to burn on the TMC222 chip, to get the new given slave-address. After that, the OTP bits are burned into the Chip.
* @param new_address the new slave address
* @return success: 1, failure: -1
*
* @~german 
* @brief Brennt die übergebene slave Adresse.
*
* Diese Funktion berechnet die benötigten Bits, um die neue Adresse auf den Chip zu brennen. Anschließend werden die OTP bits gebrannt.
* Achtung: Sind die Bits einmal gesetzt, kann dies nicht wieder rückgängig gemacht werden. Falls eine Adresse eingegeben wird, die nicht möglich ist, wird dies von der Funktion gemeldet.
* 
* @param new_address die neue Slave Adresse
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::burnNewAddress(int new_address){
	
	//SetOTPParam
	unsigned char buffer[5];
	int new_ad = 0;
	int old_ad = 0;
	int slave_address=i2c.getAddress();
	std::string yes = "NO!";

 	if(new_address <= slave_address){
		printf("\tThe new address must be higher than the old one (0x%x)!\n",slave_address);
		return -1;
 	}
 	else if (new_address > 0x7f){
		printf("The biggest slave address of the TMC222 is 0x7f\n");
		return -1;
 	}
 	else{
  		old_ad = (slave_address & 0x1e) >> 1;
  		new_ad = (new_address & 0x1e) >> 1;
  		if(((new_ad & 0x1)<(old_ad & 0x1))|((new_ad & 0x2)<(old_ad & 0x2))|((new_ad & 0x4)<(old_ad & 0x4))|((new_ad & 0x8)<(old_ad & 0x8))){
        		printf("\tThis address could not be set, because the '1' cant be undone!\n"
        			"\told OTP AD: 0x%x\n"
        			"\tnew OTP AD: 0x%x\n",old_ad, new_ad);
        		return -1;
  		}
	  	if((new_address & 0x01) == 1){
			printf("\tThe LSB address bit is set by the jumper on the module-step\n");
			new_address --;
			printf("\tThe new address will be set to: 0x%x \n", new_address);
	  	}

	    	printf("\tIf a bit of the OTP (on time programmable) is set, it cant be undone! \n\t"
	    		"If you are sure to write the new Address (0x%x) then type 'yes' in CAPITALS\n\n\t", new_address);
	    	std::cin >> yes;
	    	if(yes == "YES"){
			buffer[0] = 0x90; //SetOTPParam
			buffer[1] = 0xff; //N/A
			buffer[2] = 0xff; //N/A
			buffer[3] = 0x02; //set AD3 AD2 AD1 AD0
			buffer[4] = (unsigned char) new_ad;

		   	if(!i2c.send(buffer, 5)){
			   	return -1;
			}
			else {
				printf("\tNew Address was successfully set to: 0x%x\n\tPlease replug the module.\n\n", new_address);
				return 1;
			}
	    	}
	  	else{
			printf("\tYou didn't type 'YES'\n");
			return -1;
	  	}
	}
}

//-------------getFullStatus-------------
/** @~english 
* @brief Get full Status 1.
*
* This funktion sends the getFullStatus1 Command to the Chip.
* @return success: 1, failure: -1
*
* @~german 
* @brief Get full Status 1.
*
* Diese Funktion sendet den getFullStatus1 Befehl an den Chip.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::getFullStatus1(){
      	if(i2c.send(0x81)){
		return 1;		
	}
	else return -1;
}

//-------------getFullStatus2-------------
/** @~english 
* @brief Get full Status 2.
*
* This funktion sends the getFullStatus2 Command to the Chip.
* @return success: 1, failure: -1
*
* @~german 
* @brief Get full Status 2.
*
* Diese Funktion sendet den getFullStatus2 Befehl an den Chip.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::getFullStatus2(){
	if(i2c.send(0xfc)){
		return 1;
	}
	else return -1;
}

//-------------runInit-------------
/** @~english 
* @brief Run Init.
*
* This funktion sends the runInit Command to the Chip.
* @return success: 1, failure: -1
*
* @~german 
* @brief Run Init.
*
* Diese Funktion sendet den runInit Befehl an den Chip.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::runInit(){
		if(i2c.send(0x88)){
		return 1;
		}
		else return -1;
}

//-------------setMotorParam-------------
/** @~english 
* @brief Set motor parameter.
*
* This function sends the motor parameter to the module.
* @return success: 1, failure: -1
*
* @~german 
* @brief Setze Motor Parameter.
*
* Diese Funktion sendet die eingestellten Motor Parameter an das Modul.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::setMotorParam(){
	unsigned char buffer[8];
	//SetMotorParam
	buffer[0] = 0x89; //SetMotorParam
	buffer[1] = 0xff; //N/A
	buffer[2] = 0xff; //N/A
	buffer[3] = (unsigned char) ((irun * 0x10) + ihold); //Irun & I hold
	buffer[4] = (unsigned char) ((vmax * 0x10) + vmin); //Vmax & Vmin 
	buffer[5] = 0x00; //status
	buffer[6] = 0x00; //securePos
	buffer[7] = 0x00; //StepMode

    if(i2c.send(buffer, 8)){
	return 1;
	}
	else return -1;
}

//-------------setMotorParam-------------
/** @~english 
* @brief Set motor parameter.
*
* This function sends the motor parameter to the module.
* @param newIrun
* @param newIhold
* @param newVmax
* @param newVmin
* @return success: 1, failure: -1
*
* @~german 
* @brief Setze Motor Parameter.
*
* Diese Funktion sendet die übergebenen Motor Parameter an das Modul.
* @param newIrun
* @param newIhold
* @param newVmax
* @param newVmin
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::setMotorParam(unsigned int newIrun, unsigned int newIhold, unsigned int newVmax, unsigned int newVmin){
	irun=newIrun;
	ihold=newIhold;
	vmax=newVmax;
	vmin=newVmin;

	unsigned char buffer[8];
	//SetMotorParam
	buffer[0] = 0x89; //SetMotorParam
	buffer[1] = 0xff; //N/A
	buffer[2] = 0xff; //N/A
	buffer[3] = (unsigned char) ((irun * 0x10) + ihold); //Irun & I hold
	buffer[4] = (unsigned char) ((vmax * 0x10) + vmin); //Vmax & Vmin 
	buffer[5] = 0x00; //status
	buffer[6] = 0x00; //securePos
	buffer[7] = 0x00; //StepMode

    if(i2c.send(buffer, 8)){
	return 1;
	}
	else return -1;
}

//-------------hardStop-------------
/** @~english 
* @brief Hard stop.
*
* This funktion sends the hardStop Command to the Chip. The motor will stop immediately.
* @return success: 1, failure: -1
*
* @~german 
* @brief Hard Stop.
*
* Diese Funktion sendet den hardStop Befehl an den Chip. Der Motor bleibt sofort Stehen.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::hardStop(){
		if(i2c.send(0x85)){
		return 1;
		}
		else return -1;
}

//-------------softStop-------------
/** @~english 
* @brief Soft stop.
*
* This funktion sends the softStop Command to the Chip. The motor slows down and stops.
* @return success: 1, failure: -1
*
* @~german 
* @brief Soft Stop.
*
* Diese Funktion sendet den SoftStop Befehl an den Chip. Der Motor fährt langsam herrunter.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::softStop(){
		if(i2c.send(0x8f)){
		return 1;
		}
		else return -1;
}

//-------------resetPosition-------------
/** @~english 
* @brief Reset Position.
*
* This funktion sends the resetPosition Command to the Chip. The internal position counter of the chip is reset to 0.
* @return success: 1, failure: -1
*
* @~german 
* @brief Reset Position.
*
* Diese Funktion sendet den resetPosition Befehl an den Chip. Der interne Positions Zähler des Chips wird auf 0 zurückgesetzt.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::resetPosition(){
		if(i2c.send(0x86)){
		return 1;
		}
		else return -1;
}

//-------------setPosition-------------
/** @~english 
* @brief Set Position.
*
* This funktion sends the Position Command to the Chip. The motor drives to the given position.
* @return success: 1, failure: -1
*
* @~german 
* @brief Setze Position.
*
* Diese Funktion sendet den Position Befehl an den Chip. Der Motor fährt an die übergebene Position.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::setPosition(int position){
	unsigned char buffer[5];
	buffer[0] = 0x8B;   // SetPosition Command
	buffer[1] = 0xff;   // not avialable
	buffer[2] = 0xff;   // not avialable
	buffer[3] = (unsigned char) (position >> 8);  // PositionByte1 (15:8)
	buffer[4] = (unsigned char)  position;       // PositionByte2 (7:0)
	
	if(i2c.send(buffer, 5)){
		return 1;
	}
	else return -1;
}

//-------------drive-------------
/** @~english 
* @brief Drive.
*
* This Funktion reads the actual position from the motor and adds the amount of given steps to drive. So you can let the motor drive an amount of steps, without heaving trouble with the absolute positions.
* @return success: 1, failure: -1
*
* @~german 
* @brief Fahre.
*
* Diese Funktion ließt die aktuelle Position des Motors und addiert die anzahl der übergebenen Schritte. So kann man den Motor einfach um eine bestimmte Anzahl Schritte fahren lassen, ohne sich über die absoulute Position gedanken machen zu müssen.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_step::drive(int steps){
	int old_position;
	int new_position;

	old_position = getActualPosition();
	new_position = old_position+steps;
	if(setPosition(new_position)){
		return 1;
	}
	else return -1;
}

//-------------getMotionStatus-------------
/** @~english 
* @brief Get motion status.
*
* This funktion Indicates the actual behavior of the position controller. <br>
*	0: Actual Position = Target Position; Velocity= 0 <br>
*	1: Positive Acceleration; Velocity > 0 <br>
*	2: Negative Acceleration; Velocity > 0 <br>
*	3: Acceleration = 0 Velocity = maximum pos Velocity <br>
*	4: Actual Position /= Target Position; Velocity= 0 <br>
*	5: Positive Acceleration; Velocity < 0 <br>
*	6: Positive Acceleration; Velocity < 0 <br>
*	7: Acceleration = 0 Velocity = maximum neg Velocity <br>
*	-1: failure
*
* @return motionStatus
*
* @~german 
* @brief Bewegungs-Status.
*
* Diese Funktion gibt den aktuellen bewegungs Status des module-step zurück. <br>
*	0: Aktuelle Position = Ziehl Position; Geschwindigkeit= 0 <br>
*	1: Positive Beschleunigung; Geschwindigkeit > 0 <br>
*	2: Negative Beschleunigung; Geschwindigkeit > 0 <br>
*	3: Beschleunigung = 0; Geschwindigkeit = maximum pos Geschwindigkeit <br>
*	4: Aktuelle Position /= Ziehl Position; Geschwindigkeit= 0 <br>
*	5: Positive Beschleunigung; Geschwindigkeit < 0 <br>
*	6: Positive Beschleunigung; Geschwindigkeit < 0 <br>
*	7: Acceleration = 0 Geschwindigkeit = maximum neg Geschwindigkeit  <br>
*	-1: Fehler
* @return motionStatus
*/
int gnublin_module_step::getMotionStatus(){
	unsigned char RxBuf[8];
	int motionStatus = -1;
	getFullStatus1();
	
    	if(!i2c.receive(RxBuf, 8))
		return -1;
	motionStatus = (RxBuf[5] & 0xe0) >> 5;
	return motionStatus;
	
}


//-------------------get Switch status----------------
/** @~english 
* @brief Get switch condition.
*
* This function checks the status of the connected switch and returns the value. <br>
* Switch closed: 1 <br>
* Switch open: 0 <br>
* Failure: -1
* @return success: swi
*
* @~german 
* @brief Setze Position.
*
* Diese Funktion überprüft den Zustand des angeschlossenen Schalters und gibt den Wert zurück. <br>
* Schalter geschlossen: 1 <br>
* Schalter offen: 0 <br>
* Fehler: -1
* @return swi
*/
int gnublin_module_step::getSwitch(){
	unsigned char RxBuf[8];    	
	int swi = 0;

    	getFullStatus1();

    	if(i2c.receive(RxBuf, 8)){
	
			if(RxBuf[5] & 0x10){
				swi = 1;				
			}
			else{
				swi = 0;
			}
			return swi;
		}	
		else return -1;
}

//-------------------getActualPosition----------------
/** @~english 
* @brief Get actual position.
*
* This funktion sends the getActualPosition Command to the Chip and returns its actiual position.
* @return actualPosition -1 if failure
*
* @~german 
* @brief Aktuelle Position ausgeben.
*
* Diese Funktion sendet den getActualPosition Befehl an den Chip und gibt dessen aktuelle Position zurück.
* @return actualPosition -1 bei Fehler
*/
int gnublin_module_step::getActualPosition(){
	unsigned char RxBuf[8];	
	int actualPosition=-1;
	
	if(getFullStatus2()==-1)
		return -1;
	
	if(i2c.receive(RxBuf, 8)){
		actualPosition = (RxBuf[1]<<8 | RxBuf[2]);
	}	
	return actualPosition;
}


//*******************************************************************
//Class for accessing GNUBLIN Module-LCD 4x20
//*******************************************************************

/** @~english 
* @brief Reset the ErrorFlag.
*
* @~german 
* @brief Setzt das ErrorFlag zurück.
*
*/
gnublin_module_lcd::gnublin_module_lcd() 
{
	version = (char *) "0.3";
	error_flag=false;
}

//-------------getErrorMessage-------------
/** @~english 
* @brief Get the last Error Message.
*
* This Funktion returns the last Error Message, which occurred in that Class.
* @return ErrorMessage as c-string
*
* @~german 
* @brief Gibt die letzte Error Nachricht zurück.
*
* Diese Funktion gibt die Letzte Error Nachricht zurück, welche in dieser Klasse gespeichert wurde.
* @return ErrorMessage als c-string
*/
const char *gnublin_module_lcd::getErrorMessage(){
	return ErrorMessage.c_str();
}

//-------------fail-------------
/** @~english 
* @brief Returns the error flag. 
*
* If something went wrong, the flag is true.
* @return bool error_flag
*
* @~german 
* @brief Gibt das Error Flag zurück.
*
* Falls das Error Flag in der Klasse gesetzt wurde, wird true zurück gegeben, anderenfalls false.
* @return bool error_flag
*/
bool gnublin_module_lcd::fail(){
	return error_flag;
}

//-------------setAddress-------------
/** @~english 
* @brief Set the slave address 
*
* With this function you can set the individual I2C Slave-Address of the module.
* @param Address new I2C slave Address
*
* @~german 
* @brief Gibt das Error Flag zurück.
*
* Mit dieser Funktion kann die individuelle I2C Slave-Adresse des Moduls gesetzt werden.
* @param Address neue I2C slave Adresse
*/
void gnublin_module_lcd::setAddress(int Address){
	pca.setAddress(Address);
}

//-------------setDevicefile-------------
/** @~english 
* @brief Set devicefile.
*
* With this function you can change the I2C device file. Default is "/dev/i2c-1"
* @param filename path to the I2C device file
*
* @~german 
* @brief Setzt Device Datei.
*
* Mit dieser Funktion kann die I2C Gerätedatei geändert werden. Standartmäßig wird "/dev/i2c-1" benutzt.
* @param filename Pfad zur I2C Gerätedatei
*/
void gnublin_module_lcd::setDevicefile(std::string filename){
	pca.setDevicefile(filename);
}

//-------------out-------------
/** @~english 
* @brief LCD out.
*
* This Funktion sends the given data and RS/RW pins to the pca9555 chip.
* @param rsrw contains the RS/RW pins
* @param data contains the data to send
* @return success: 1, failure: -1
*
* @~german 
* @brief Sendet Daten
*
* Diese Funktion sendet die übergebenen Daten und die RS/RW pins an den pca9555.
* @param rsrw beinhalten die RS/RW pins
* @param data Daten die an das Display gesendet werden sollen
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::out(unsigned char rsrw, unsigned char data ){
	if(!pca.writePort(0, data)){			//send data on Port 0
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}
	if(!pca.writePort(1, rsrw)){			//send RS/RW bits on Port 1
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}
	if(!pca.writePort(1, (rsrw | LCD_EN))){	//enable on
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}
	usleep(LCD_ENABLE_US);
	if(!pca.writePort(1, rsrw)){			//enable off
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}
	return 1;
}

//-------------sendData-------------
/** @~english 
* @brief Send Data to the LCD
*
* This Funktion sends the given char to the display.
* @param data contains the data to send
* @return success: 1, failure: -1
*
* @~german 
* @brief Sendet Daten an das Display.
*
* Diese Funktion sendet die übergebenen Daten an das Display.
* @param data Daten die an das Display gesendet werden sollen
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::sendData(unsigned char data){
        if(!out(LCD_RS, (unsigned char) data)){
			return -1;
		}
        usleep(LCD_WRITEDATA_US);
        return 1;
}

//-------------command-------------
/** @~english 
* @brief Send command to the LCD
*
* This Funktion sends an command to the display.
* @param data the command to send
* @return success: 1, failure: -1
*
* @~german 
* @brief Sendet einen Befehl an das Display.
*
* Diese Funktion sendet die übergebenen Befehle an das Display.
* @param data Befehl die an das Display gesendet werden sollen
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::command(unsigned char data){
        if(!out(0x00, (unsigned char) data)){
		    return -1;
		}
        usleep(LCD_COMMAND_US);
        return 1;
}

//-------------clear-------------
/** @~english 
* @brief Clear the LCD.
*
* Everythin on the Display will be erased
* @return success: 1, failure: -1
*
* @~german 
* @brief Bereinigt das Display
*
* Alles was auf dem Display angezeigt wird, wird gelöscht.
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::clear(){
        if(!command(LCD_CLEAR_DISPLAY)){
			return -1;
		}
        usleep(LCD_CLEAR_DISPLAY_MS);
        return 1;
}

//-------------home-------------
/** @~english 
* @brief Curse home command.
*
* Returns the Cursor to the first possition.
* @return success: 1, failure: -1
*
* @~german 
* @brief Setzt den Cursor auf Anfangsposition oben links  
*
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::home(){
        if(!command(LCD_CURSOR_HOME)){
			return -1;
		}
        usleep(LCD_CURSOR_HOME_MS);
        return 1;
}

//-------------setdisplay-------------
/** @~english 
* @brief Set display command.
*
* @return success: 1, failure: -1
*
* @~german 
* @brief Display Einstellungen Befehl.
*
* Mit dieser Funktion können Cursor-Einstellungen am Display vorgenommen werden. Wenn die Übergabe cursor == 1 ist, wird der Cursor am Display an der momentanen Stelle angezeigt(_). Wird blink == 1 übergeben, blinkt ein viereckiger Cursor. 
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::setdisplay(int cursor, int blink){
        unsigned char set_display;
        //Display ON/OFF Control
        set_display = LCD_SET_DISPLAY + LCD_DISPLAY_ON;
        if(cursor) set_display = set_display + LCD_CURSOR_ON;
        if(blink) set_display = set_display + LCD_BLINKING_ON;
        if(!command(set_display))
        	return -1;
        return 1;
}

//-------------setcursor-------------
/** @~english 
* @brief Set Cursor Command.
*
* Sets the Cursor to the given Position on the Display.
* @param x The Line 
* @param y The Column 
* @return success: 1, failure: -1
*
* @~german 
* @brief Setzt den Cursor an die übergebene Position auf das Display. 
*
* @param x Die Zeile 
* @param y Die Spalte
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::setcursor(unsigned char x, unsigned char y){
        unsigned char data;
        switch(x){
                case 1: //1. Zeile
                        data = LCD_SET_DDADR + LCD_DDADR_LINE1 + y;
                        break;

                case 2:    // 2. Zeile
                        data = LCD_SET_DDADR + LCD_DDADR_LINE2 + y;
                        break;

                case 3:    // 3. Zeile
                        data = LCD_SET_DDADR + LCD_DDADR_LINE3 + y;
                        break;

                case 4:    // 4. Zeile
                        data = LCD_SET_DDADR + LCD_DDADR_LINE4 + y;
                         break;

                default:
                		error_flag=true;
						ErrorMessage = "Wrong line/column";
                        return -1;
        }
        if(!command(data)){
        	return -1;
        }
        return 1;
}

//-------------string-------------
/** @~english 
* @brief Sends the string to the display.
*
* @param data string to send 
* @return success: 1, failure: -1
*
* @~german 
* @brief Sendet den String an das Display. 
*
* Dieser Funktion kann ein String übergeben werden, welcher auf dem Display angezeigt werden soll. 
* @param data String zum senden 
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::string(const char *data){
        while(*data != '\0'){
                if(!sendData( *data++)){
                	return -1;
                }
        }
        return 1;
}

//-------------init-------------
/** @~english 
* @brief Initializes the LCD.
*
* @return success: 1, failure: -1
*
* @~german 
* @brief Initialisiert das LCD.
*
* Wenn diese Funktion aufgerufen wird, wird der pca9555 so eingestellt, um das Display ansteuern zu können. Außerdem werden einige Befehle an das Display gesendet, um es auf das Anzeigen von Text vorbereitet. 
* @return Erfolg: 1, Fehler: -1
*/
int gnublin_module_lcd::init(){
	//Set Ports as output
	if(!pca.portMode(0, "out")){ 	//Port 0 as Output
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}
	if(!pca.portMode(1, "out")){ 	//Port 1 as Output
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}

	//// initial alle Ausgänge auf Null
	if(!pca.writePort(0, 0x00)){
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}
	if(!pca.writePort(1, 0x00)){
		error_flag=true;
		ErrorMessage = pca.getErrorMessage();
		return -1;
	}
	
	usleep(LCD_BOOTUP_MS);

	//function set
	if(!command(LCD_SET_FUNCTION |
                        LCD_FUNCTION_8BIT |
                        LCD_FUNCTION_2LINE |
                        LCD_FUNCTION_5X7)){
		return -1;
	}
	usleep(LCD_SET_8BITMODE_MS);

	//Display ON/OFF Control
	if(!setdisplay(0, 0)){
		return -1;
	}

	if(!clear()){
		return -1;
	}
	
	//entry mode set
	if(!command(LCD_SET_ENTRY |
			LCD_ENTRY_INCREASE |
			LCD_ENTRY_NOSHIFT)){
		return -1;
	}
	
	return 1;
}
