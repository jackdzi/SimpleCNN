const express = require("express");
const { exec } = require("child_process");
const cors = require("cors");

const app = express();
app.use(express.json());
app.use(cors());


app.post("/run", (req, res) => {
  const input = req.body.input;
  console.log("Request recieved")

  const process = exec("./parser", (error, stdout, stderr) => {
    if (error) return res.status(500).send(`Error: ${error.message}`);
    if (stderr) return res.status(500).send(`Stderr: ${stderr}`);
    res.send({ output: stdout.trim() });
  });
  process.stdin.write(input);
  process.stdin.end();
});

const PORT = 3001;
app.listen(PORT, () =>
  console.log(`Server running on http://localhost:${PORT}`),
);
