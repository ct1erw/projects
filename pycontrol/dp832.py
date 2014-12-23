import visa
rm = visa.ResourceManager()
rm.list_resources()
inst = rm.open_resource('TCPIP0::192.168.1.88::INSTR')
# print "CH1"
# print(inst.query(":MEASURE:ALL? CH1"))

# print values from 3 channels
channels = ['CH1','CH2','CH3']
for channel in enumerate(channels):
	query = ":MEASURE:ALL? " + channel[1]
	print "{ch},{val}".format(ch=channel[1], val=inst.query(query))
