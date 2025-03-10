import { useRef, useState } from "react";
import { ReactSketchCanvas } from "react-sketch-canvas";
import { useVectorContext } from "../../assets/imageData";
import DataDisplay from "../DataDisplay/DataDisplay";

const Canvas: React.FC = () => {
  const canvasRef = useRef(null);
  const { vectorData, setVectorData } = useVectorContext();
  const [output, setOutput] = useState("[0,0,0,0,0,0,0,0,0,0]");

  const handleUpdate = async (data: string) => {
    await new Promise((resolve) => setTimeout(resolve, 0));
    setVectorData(data);
  };

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
      console.log(data);
      setOutput(data.output);
    } catch (error) {
      console.error("Error:", error);
      setOutput("Error running program");
    }
  };

  const handleMouseUp = async () => {
    await new Promise((resolve) => setTimeout(resolve, 0));
    (canvasRef.current as any)
      .exportImage("png")
      .then((data: string) => {
        handleUpdate(data);
      })
      .catch((e: any) => {
        console.log(e);
      });
  };

  const handleClear = () => {
    (canvasRef.current as any).clearCanvas();
  };

  return (
    <div className="flex flex-col items-center p-10" onMouseUp={handleMouseUp}>
      <ReactSketchCanvas
        ref={canvasRef}
        style={{ border: "4px solid #63b3ed", width: 280, height: 280 }}
        strokeWidth={25}
        strokeColor="black"
      />
      <div className="flex space-x-4 mt-5">
        <button
          className="bg-gradient-to-r from-green-400 to-blue-500 hover:from-blue-500 hover:to-green-400 text-white font-bold py-2 px-4 rounded shadow-lg focus:outline-none focus:ring-2 focus:ring-blue-400"
          onClick={async () => {
            handleClear();
            await new Promise((resolve) => setTimeout(resolve, 0));
            handleMouseUp();
          }}
        >
          Clear Canvas
        </button>
        <button
          className="bg-gradient-to-r from-green-400 to-blue-500 hover:from-blue-500 hover:to-green-400 text-white font-bold py-2 px-4 rounded shadow-lg focus:outline-none focus:ring-2 focus:ring-blue-400"
          onClick={handleRunProgram}
        >
          Run
        </button>
      </div>
    <div className="flex flex-col justify-center items-center">
      <div className="mx-auto">
        <h1 className="text-2xl font-bold text-white m-4">Output:</h1>
        <div className="grid place-items-center gap-0.5 grid-cols-[repeat(28,_min-content)] grid-rows-[repeat(28,_min-content)]">
          <DataDisplay>{output}</DataDisplay>
        </div>
      </div>
    </div>
    </div>
  );
};

export default Canvas;
