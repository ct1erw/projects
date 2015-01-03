import visa
rm = visa.ResourceManager()
rm.list_resources()
inst = rm.open_resource('TCPIP0::192.168.1.30::INSTR')
print(inst.query("*IDN?"))
