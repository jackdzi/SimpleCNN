// src/Canvas.js
import React, { useRef } from 'react';
import CanvasDraw from 'react-canvas-draw';

const Canvas = ({ saveImage }) => {
  const canvasRef = useRef(null);

  const handleSave = () => {
    const canvas = canvasRef.current.canvas.drawing;
    const ctx = canvas.getContext('2d');

    // Get the pixel data from the canvas
    const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);

    // Pass the image data to the parent component
    saveImage(imageData);
  };

  const handleClear = () => {
    canvasRef.current.clear();
  };

  return (
    <div>
      <CanvasDraw
        ref={canvasRef}
        brushColor="#000000"
        brushRadius={10}
        canvasWidth={280}
        canvasHeight={280}
        lazyRadius={0}
      />
      <button onClick={handleSave}>Save Drawing</button>
      <button onClick={handleClear}>Clear Drawing</button>
    </div>
  );
};

export default Canvas;
