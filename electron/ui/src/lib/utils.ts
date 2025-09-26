import { type ClassValue, clsx } from "clsx";
import { twMerge } from "tailwind-merge";

// cn: merge conditional classNames with tailwind-merge to dedupe conflicts
export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs));
}

