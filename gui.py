import os
import subprocess
import tkinter as tk
from tkinter import filedialog, messagebox, ttk

APP_TITLE = "Huffman Compression Tool"
EXE_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hzip.exe")


class HuffmanGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title(APP_TITLE)
        self.geometry("640x360")
        self.minsize(560, 320)

        self.input_path = tk.StringVar()
        self.output_path = tk.StringVar()
        self.operation = tk.StringVar(value="compress")
        self.status = tk.StringVar(value="Choose a file to begin.")

        self._build_ui()

    def _build_ui(self):
        frame = ttk.Frame(self, padding=18)
        frame.pack(fill="both", expand=True)
        frame.columnconfigure(1, weight=1)

        ttk.Label(frame, text="Input file").grid(row=0, column=0, sticky="w", pady=6)
        ttk.Entry(frame, textvariable=self.input_path).grid(
            row=0, column=1, sticky="ew", padx=8
        )
        ttk.Button(frame, text="Browse…", command=self.choose_input).grid(
            row=0, column=2
        )

        ttk.Label(frame, text="Operation").grid(row=1, column=0, sticky="w", pady=6)
        operation_box = ttk.Combobox(
            frame,
            textvariable=self.operation,
            values=("compress", "decompress"),
            state="readonly",
            width=18,
        )
        operation_box.grid(row=1, column=1, sticky="w", padx=8)

        ttk.Label(frame, text="Output file").grid(row=2, column=0, sticky="w", pady=6)
        ttk.Entry(frame, textvariable=self.output_path).grid(
            row=2, column=1, sticky="ew", padx=8
        )
        ttk.Button(frame, text="Save as…", command=self.choose_output).grid(
            row=2, column=2
        )

        ttk.Button(frame, text="Run", command=self.run_operation).grid(
            row=3, column=1, sticky="w", padx=8, pady=16
        )

        ttk.Label(frame, text="Result").grid(row=4, column=0, sticky="nw")
        self.result = tk.Text(frame, height=5, wrap="word", state="disabled")
        self.result.grid(row=4, column=1, columnspan=2, sticky="nsew", padx=8)
        frame.rowconfigure(4, weight=1)

        ttk.Label(frame, textvariable=self.status).grid(
            row=5, column=0, columnspan=3, sticky="w", pady=(12, 0)
        )

    def choose_input(self):
        path = filedialog.askopenfilename(title="Select input file")
        if path:
            self.input_path.set(path)
            if not self.output_path.get():
                suffix = ".huf" if self.operation.get() == "compress" else ".restored"
                self.output_path.set(path + suffix)

    def choose_output(self):
        path = filedialog.asksaveasfilename(title="Choose output file")
        if path:
            self.output_path.set(path)

    def show_result(self, text):
        self.result.configure(state="normal")
        self.result.delete("1.0", "end")
        self.result.insert("1.0", text)
        self.result.configure(state="disabled")

    def run_operation(self):
        source = self.input_path.get().strip()
        destination = self.output_path.get().strip()

        if not source or not os.path.isfile(source):
            messagebox.showerror("Input required", "Select an existing input file.")
            return
        if not destination:
            messagebox.showerror("Output required", "Choose an output file.")
            return
        if os.path.abspath(source) == os.path.abspath(destination):
            messagebox.showerror("Invalid output", "Input and output files must differ.")
            return
        if os.path.exists(destination):
            if not messagebox.askyesno(
                "Overwrite file?",
                f"{destination} already exists. Replace it?",
            ):
                self.status.set("Cancelled; existing file was not changed.")
                return

        if not os.path.isfile(EXE_PATH):
            messagebox.showerror(
                "Executable not found",
                f"Could not find hzip.exe here:\n{EXE_PATH}\n\nBuild it first.",
            )
            return

        command = [EXE_PATH, self.operation.get(), source, destination]
        try:
            completed = subprocess.run(
                command, capture_output=True, text=True, check=False
            )
        except OSError as error:
            messagebox.showerror("Could not start program", str(error))
            return

        output = (completed.stdout or "").strip()
        error = (completed.stderr or "").strip()

        if completed.returncode == 0:
            self.status.set("Completed successfully.")
            self.show_result(output or "Operation completed.")
        else:
            self.status.set("Operation failed.")
            self.show_result(error or output or "The operation failed.")
            messagebox.showerror("Huffman operation failed", error or output)


if __name__ == "__main__":
    HuffmanGUI().mainloop()
