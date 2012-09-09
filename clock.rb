#!/usr/bin/ruby
require 'rubygems'
require 'serialport'

port = "/dev/ttyUSB0"    #arduinoのポート番号
sp = SerialPort.new(port, 9600, 8, 1, SerialPort::NONE)

sp.write "T" + Time.now.to_i.to_s
puts Time.now

#sp.write( "T" + Time.now.to_i.to_s)

while true
  c = sp.getc
  if c == 7
    sp.write( "T" + Time.now.to_i.to_s)
    puts Time.now
  elsif c != nil
    format("%c",c)
  end
end

sp.close
