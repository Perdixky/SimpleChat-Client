import { useEffect, useState } from 'react';
import type { ReactNode } from 'react';

export interface ModalProps {
  isOpen: boolean;
  onClose: () => void;
  title?: string;
  children: ReactNode;
  maxWidth?: 'sm' | 'md' | 'lg' | 'xl';
  showCloseButton?: boolean;
  closeOnOverlayClick?: boolean;
  className?: string;
}

export function Modal({
  isOpen,
  onClose,
  title,
  children,
  maxWidth = 'md',
  showCloseButton = true,
  closeOnOverlayClick = true,
  className = ''
}: ModalProps) {
  const [isClosing, setIsClosing] = useState(false);
  const [isAnimating, setIsAnimating] = useState(false);

  const maxWidthClasses = {
    sm: 'max-w-sm',
    md: 'max-w-md',
    lg: 'max-w-lg',
    xl: 'max-w-xl'
  };

  const handleClose = () => {
    setIsClosing(true);
    setTimeout(() => {
      onClose();
      setIsClosing(false);
      setIsAnimating(false);
    }, 300); // Match animation duration
  };

  const handleOverlayClick = () => {
    if (closeOnOverlayClick) {
      handleClose();
    }
  };

  // Handle modal open animation with slight delay for smooth transition
  useEffect(() => {
    if (isOpen) {
      // Small delay to ensure the modal is rendered before starting animation
      const timer = setTimeout(() => {
        setIsAnimating(true);
      }, 10);
      return () => clearTimeout(timer);
    } else {
      setIsAnimating(false);
    }
  }, [isOpen]);

  // Handle ESC key
  useEffect(() => {
    const handleEscKey = (e: KeyboardEvent) => {
      if (e.key === 'Escape' && isOpen) {
        handleClose();
      }
    };

    if (isOpen) {
      document.addEventListener('keydown', handleEscKey);
      // Prevent body scroll when modal is open
      document.body.style.overflow = 'hidden';
    }

    return () => {
      document.removeEventListener('keydown', handleEscKey);
      document.body.style.overflow = '';
    };
  }, [isOpen]);

  if (!isOpen) return null;

  return (
    <div className={`fixed inset-0 z-50 flex items-center justify-center p-4 transition-all duration-300 ${
      isClosing ? 'animate-out fade-out' : 'animate-in fade-in'
    }`}>
      {/* Backdrop with smooth blur transition */}
      <div
        className={`absolute inset-0 modal-backdrop ${
          isAnimating && !isClosing
            ? 'modal-backdrop-active'
            : isClosing
            ? 'modal-backdrop-closing'
            : ''
        }`}
        onClick={handleOverlayClick}
      />

      {/* Modal content */}
      <div className={`relative w-full ${maxWidthClasses[maxWidth]} bg-white/95 backdrop-blur-md rounded-2xl shadow-2xl border border-gray-200 transition-all duration-300 ${
        isClosing
          ? 'animate-out zoom-out-95 slide-out-to-bottom-4'
          : 'animate-in zoom-in-95 slide-in-from-bottom-4'
      } ${className}`}>
        <div className="p-6">
          {/* Header */}
          {(title || showCloseButton) && (
            <div className="flex items-center justify-between mb-4">
              {title && (
                <h3 className="text-xl font-semibold text-gray-900">{title}</h3>
              )}
              {showCloseButton && (
                <button
                  onClick={handleClose}
                  className="p-2 hover:bg-gray-100 rounded-lg transition-colors duration-200 ml-auto"
                  aria-label="关闭"
                >
                  <svg className="w-5 h-5 text-gray-500" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                  </svg>
                </button>
              )}
            </div>
          )}

          {/* Content */}
          <div className="modal-content">
            {children}
          </div>
        </div>
      </div>
    </div>
  );
}

export default Modal;