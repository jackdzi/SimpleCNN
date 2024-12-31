import { useRef } from "react";
import { ReactSketchCanvas } from "react-sketch-canvas";
import { useVectorContext } from "../../assets/imageData";

const Canvas: React.FC = () => {
  const canvasRef = useRef(null);
  const { setVectorData } = useVectorContext();

  const handleUpdate = async (data: string) => {
    await new Promise((resolve) => setTimeout(resolve, 0));
    setVectorData(data);
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
        strokeWidth={20}
        strokeColor="black"
      />
      <button
        className="mt-5 bg-gradient-to-r from-green-400 to-blue-500 hover:from-blue-500 hover:to-green-400 text-white font-bold py-2 px-4 rounded shadow-lg focus:outline-none focus:ring-2 focus:ring-blue-400"
        onClick={async () => {
          handleClear();
          await new Promise((resolve) => setTimeout(resolve, 0));
          handleMouseUp();
        }}
      >
        Clear Canvas
      </button>
    </div>
  );
};

export default Canvas;
