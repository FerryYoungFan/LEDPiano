def printMidiTable(indent=0, gap=2, startCode=0, endCode=127, A4=440):
    headers = ["MIDI Code", "HEX Code", "Piano Key", "Note Name", "Frequency (Hz)"]
    midiTable = [headers]
    cellSize = []
    noteNameList = ["C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"]

    for header in headers:
        cellSize.append(len(header))

    for code in range(endCode, startCode-1, -1):
        keyNum = code - 20
        if keyNum > 88 or keyNum < 1:
            keyNum = "--"
        noteName = noteNameList[code % 12]
        octave = int(code / 12) - 1
        noteName = str(noteName) + str(octave)
        freq = round(A4 * 2 ** ((code - 69) / 12), 2)
        row = [str(code), "0x{:02X}".format(code), str(keyNum), noteName, "{:.2f}".format(freq)]
        for i, cell in enumerate(row):
            if len(cell) > cellSize[i]:
                cellSize[i] = len(cell)
        midiTable.append(row)

    for row in midiTable:
        rowStr = " " * indent
        for i, cell in enumerate(row):
            rowStr += cell + " " * (cellSize[i] - len(cell))
            if i < len(row) - 1:
                rowStr += " " * gap
        print(rowStr)


if __name__ == '__main__':
    printMidiTable(indent=3)