import React, { useState } from "react";
import { useVectorContext } from "../../assets/imageData";

const CppLink: React.FC = () => {
  const [output, setOutput] = useState("");
  const { vectorData } = useVectorContext();

  const handleRunProgram = async () => {
    try {
      const response = await fetch("http://localhost:3001/run", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ input: vectorData }),
      });

      const data = await response.json();
      setOutput(data.output);
    } catch (error) {
      console.error("Error:", error);
      setOutput("Error running program");
    }
  };

  return (
    <div className="flex flex-col justify-center items-center">
      <button
        className="bg-gradient-to-r from-green-400 to-blue-500 hover:from-blue-500 hover:to-green-400 text-white font-bold py-2 px-4 rounded shadow-lg focus:outline-none focus:ring-2 focus:ring-blue-400"
        onClick={() => {
          handleRunProgram();
        }}
      >
        Run
      </button>
      <div className="mx-auto">
        <h1 className="text-2xl font-bold text-white m-4">Output:</h1>
        <div className="grid place-items-center gap-0.5 grid-cols-[repeat(28,_min-content)] grid-rows-[repeat(28,_min-content)]">
          {output.substring(31, output.length).split("\n").map((row, indexrow) =>
            row.split(" ").map((value, index) => (
              <div
                key={index + indexrow * 28}
                style={{
                  backgroundColor: `rgb(${value}, ${value}, ${value})`,
                  width: "0.75em",
                  height: "0.75em",
                }}
              ></div>
            )),
          )}
        </div>
      </div>
    </div>
  );
};

export default CppLink;
