import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import App from "./App.tsx";
import { VectorProvider } from "./assets/imageData";

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <VectorProvider>
      <App />
    </VectorProvider>
  </StrictMode>,
);
