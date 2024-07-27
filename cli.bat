@echo off
arduino-cli lib install "LinkedList" 
arduino-cli compile --fqbn arduino:avr:nano --clean
