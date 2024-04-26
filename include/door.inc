<?php
class Door {
    private $dest;
    private $dir;
    private $x;

    public static $dirUp = "UP";
    public static $dirDown = "DOWN";

    public function __construct($x, $dest, $dir) {
        $this->x = $x;
        $this->dest = $dest;
        $this->dir = $dir;
    }

    public function getDir() {
        return $this->dir;
    }

    public function getDest() {
        return $this->dest;
    }

    public function getX() {
        return $this->x;
    }
}