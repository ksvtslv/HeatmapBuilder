import matplotlib
import matplotlib.pyplot as plt

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import ttkbootstrap as ttk
from ttkbootstrap.constants import *
from tkinter import filedialog

import numpy as np

import pandas as pd

from pathlib import Path
import sys
import os

import argparse

def load_xyvalue_table(path):
    with open(path, "r") as f:
        lines = f.readlines()
    for i, line in enumerate(lines):
        if line.strip().startswith("X") and "Y" in line and "Value" in line:
            header_index = i
            break
    df = pd.read_csv(
        path,
        sep=r"\s+",
        skiprows=header_index,
        engine="python",
        encoding_errors="ignore",
    )
    return df

def generate_and_save_heatmap(inputfile, outputfile):
    df = load_xyvalue_table(inputfile)
    iks = np.array(df['X'])
    igrek = np.array(df['Y'])
    magni = np.array(df['Value'])
    step = igrek[1]-igrek[0]
    count = 0
    for i in range(iks.size):
        if iks[0] < iks[i]:
            break
        count += 1
    diks = iks.reshape((count, count))
    digrek = igrek.reshape((count, count))
    dmagni = magni.reshape((count, count))
    range_to = count+1 if count % 2 == 0 else count
    for i in range(1, range_to, 2):
        dmagni[i] = dmagni[i][::-1]
        digrek[i] = digrek[i][::-1]
    plt.clf()
    plt.pcolormesh(diks, digrek, dmagni, cmap='rainbow', shading='auto')
    plt.colorbar(label='Magnitude')
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.title('Heatmap for Regular Grid Data')
    plt.savefig(outputfile)

class App(ttk.Window):
    def __init__(self):
        super().__init__(themename="cosmo")
        self.title("Heatmap viewer")
        self.geometry("900x500")

        # ======= PanedWindow =======
        pw = ttk.Panedwindow(self, orient=HORIZONTAL)
        pw.pack(fill=BOTH, expand=YES)

        left_frame = ttk.Frame(pw, padding=10)
        right_frame = ttk.Frame(pw, padding=10)

        pw.add(left_frame, weight=1)
        pw.add(right_frame, weight=3)

        # ======= Left panel =======
        ttk.Label(left_frame, text="Control Panel", font="-size 14").pack(pady=10)

        ttk.Button(
            left_frame,
            text="Select a file",
            bootstyle=PRIMARY,
            command=self.choose_file
        ).pack(pady=10)

        self.folder_label = ttk.Label(left_frame, text="Folder is not selected", wraplength=200)
        self.folder_label.pack(pady=10)

        self.file_label = ttk.Label(left_frame, text="File is not selected", wraplength=200)
        self.file_label.pack(pady=10)

        # ======= Empty plot =======
        self.fig, self.ax = plt.subplots(figsize=(5, 4), dpi=100)
        self.ax.set_title("File is not selected")

        self.canvas = FigureCanvasTkAgg(self.fig, master=right_frame)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.pack(fill=BOTH, expand=YES)

        # For deleting old pcolormesh
        self.pcm = None

    # ======= Selecting the file =======
    def choose_file(self):
        filepath = filedialog.askopenfilename(
            title="Select a file",
            filetypes=[("Все файлы", "*.*"), ("Текстовые", "*.txt"), ("CSV", "*.csv")]
        )

        if not filepath:
            return
        
        self.folder_label.config(text=filepath[:filepath.rfind('/')])
        self.file_label.config(text=filepath.split('/')[-1])
        self.plot_pcolormesh(filepath)

    def load_xyvalue_table(self, path):
        with open(path, "r") as f:
            lines = f.readlines()
        for i, line in enumerate(lines):
            if line.strip().startswith("X") and "Y" in line and "Value" in line:
                header_index = i
                break
        df = pd.read_csv(
            path,
            sep=r"\s+",
            skiprows=header_index,
            engine="python",
            encoding_errors="ignore",
        )
        return df

    # ======= Plot the pcolormesh =======
    def plot_pcolormesh(self, filepath):
        self.ax.clear()

        # ======= Extract and parse the data =======
        df = self.load_xyvalue_table(filepath)
        iks = np.array(df['X'])
        igrek = np.array(df['Y'])
        magni = np.array(df['Value'])

        step = igrek[1]-igrek[0]

        count = 0
        for i in range(iks.size):
            if iks[0] < iks[i]:
                break
            count += 1

        diks = iks.reshape((count, count))
        digrek = igrek.reshape((count, count))
        dmagni = magni.reshape((count, count))
        range_to = count+1 if count % 2 == 0 else count
        for i in range(1, range_to, 2):
            dmagni[i] = dmagni[i][::-1]
            digrek[i] = digrek[i][::-1]

        self.pcm = self.ax.pcolormesh(diks, digrek, dmagni, cmap='rainbow', shading="auto")

        self.ax.set_title("Heatmap for Regular Grid Data")
        self.fig.colorbar(self.pcm, ax=self.ax, label='Magnitude')
        self.canvas.draw()


if __name__ == "__main__":
    if len(sys.argv) == 1:
        app = App()
        app.mainloop()
    elif len(sys.argv) == 2\
            and sys.argv[1] != '-h'\
            and sys.argv[1] != '--help':
        filepath = Path(sys.argv[1])
        if not filepath.exists():
            print(f"File {filepath} doesn't exist!")
            exit(1)
        df = load_xyvalue_table(filepath)
        iks = np.array(df['X'])
        igrek = np.array(df['Y'])
        magni = np.array(df['Value'])
        step = igrek[1]-igrek[0]
        count = 0
        for i in range(iks.size):
            if iks[0] < iks[i]:
                break
            count += 1
        diks = iks.reshape((count, count))
        digrek = igrek.reshape((count, count))
        dmagni = magni.reshape((count, count))
        range_to = count+1 if count % 2 == 0 else count
        for i in range(1, range_to, 2):
            dmagni[i] = dmagni[i][::-1]
            digrek[i] = digrek[i][::-1]
        plt.clf()
        plt.pcolormesh(diks, digrek, dmagni, cmap='rainbow', shading='auto')
        plt.colorbar(label='Magnitude')
        plt.xlabel('X-axis')
        plt.ylabel('Y-axis')
        plt.title('Heatmap for Regular Grid Data')
        plt.show()
    else:
        parser = argparse.ArgumentParser(
            prog='heatmap',
            description='Program plots or generates with saving heatmap of ...',
            epilog='Text at the bottom of help')
        
        parser.add_argument('-p', '--path', help="path to folder with data files")
        parser.add_argument('-o', '--output', help='path for storing generated heatmaps')
        parser.add_argument('-i', '--input', help='fullpath to datafile to create heatmap')
        
        args = parser.parse_args()

        if args.input is not None:
            outputfile = Path(args.output) / Path(args.input).with_suffix('.png') if args.output is not None else args.input + '.png'
            outputfile = None
            if args.output is not None:
                pout = Path(args.output)
                if not pout.exists():
                    pout.mkdir(parents=True)
                outputfile = pout / Path(args.input).with_suffix('.png')
            else:
                outputfile = args.input + '.png'
            generate_and_save_heatmap(args.input, outputfile)
        elif args.path is not None:
            path_to_files = Path(args.path)
            for item in path_to_files.iterdir():
                if item.is_file():
                    if item.suffix == '.txt':
                        outputfile = None
                        if args.output is not None:
                            pout = Path(args.output)
                            if not pout.exists():
                                pout.mkdir(parents=True)
                            outputfile = pout / (item.name + '.png')
                        else:
                            outputfile = item.with_suffix('.png')
                        try:
                            generate_and_save_heatmap(item, outputfile)
                        except Exception as e:
                            print(f"Can't process file {item}. Details: {e}")

