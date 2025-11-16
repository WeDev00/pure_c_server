@echo off
powershell -Command "Start-Process PowerShell -Verb RunAs -ArgumentList 'Stop-Service -Name postgresql-x64-17'"
