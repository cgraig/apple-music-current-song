import argparse
import math
import os
import sys
from datetime import datetime, timedelta

parser = argparse.ArgumentParser(description='Generate track listing replacing artist name with social name')

parser.add_argument('tracklist', type=str, help='The tracklist file which should have list of artist and track')
parser.add_argument('datetime', type=str, help='A file which includes date timestamps that should correlate to tracklist file')

args = parser.parse_args()

if not os.path.exists(args.tracklist):
    print(f"Could not find {args.tracklist}, exiting.")

if not os.path.exists(args.datetime):
    print(f"Could not find {args.datetime}, exiting.")
    
with open(args.tracklist, 'r') as trackListFile:
    tracks = trackListFile.readlines()
    
with open(args.datetime, 'r') as datetimeFile:
    datetimestamps = datetimeFile.readlines()
    
if len(tracks) != len(datetimestamps):
    print("File line count mismatches, exiting!")
    sys.exit(1)
    
with open('thecuefile.cue', 'w') as cueFile:
    cueFile.write("PERFORMER \"performer\"" + os.linesep)
    cueFile.write("TITLE \"Title\"" + os.linesep)
    cueFile.write("FILE \"anything.mp3\" MP3" + os.linesep)
    
    index = 0
    start_time = timedelta(0)
    for track in tracks:
        artist_name, song_title = track.split(' - ')
        artist_name = artist_name.strip()
        song_title = song_title.strip()
        
        date_time = datetimestamps[index].strip()
        index+=1
        
        timestamp = datetime.strptime(date_time, '%a %b %d %H:%M:%S %Y')
        if start_time == timedelta(0):
            start_time = timestamp

        time_since_start = timestamp - start_time
        seconds = int(time_since_start.total_seconds())
        secs_in_a_min = 60
        minutes, seconds = divmod(seconds, secs_in_a_min)
        
        cueFile.write(f"  TRACK {index:02d} AUDIO" + os.linesep)
        cueFile.write(f"    PERFORMER \"{artist_name}\"" + os.linesep)
        cueFile.write(f"    TITLE \"{song_title}\"" + os.linesep)
        cueFile.write(f"    INDEX 01 {minutes:02d}:{seconds:02d}:00" + os.linesep)
        
    cueFile.close()