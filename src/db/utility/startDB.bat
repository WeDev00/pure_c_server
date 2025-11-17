@echo off
powershell -Command "Start-Process PowerShell -Verb RunAs -ArgumentList 'Start-Service -Name postgresql-x64-17'"
